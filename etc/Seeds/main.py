import requests
from bs4 import BeautifulSoup
import queue
from urllib.parse import urlparse
import time
import pprint
from ast import literal_eval


START_URL = 'https://en.wikipedia.org/wiki/Portal:Contents/Portals'
OUTPUT_FILENAME = 'urls.txt'
TEMP_FILENAME = 'status.'


def get_num_urls():
	'''The number of urls you want to end up with.'''
	return 10000


def get_domain(url):
	'''Get the domain of the url.'''
	return urlparse(url).netloc


def is_wiki(url):
	'''Return whether the url is from wikipedia.'''
	return 'wiki' in get_domain(url) or 'wiktionary' in get_domain(url)


def is_valid(url):
	'''Check if url is valid.'''
	not_none = url is not None
	is_string = isinstance(url, str)
	not_empty = is_string and len(url) > 0
	if not_empty:
		valid_start = url[0] not in ['#']
		not_video = url.split('.')[-1] not in ['webm', 'pdf']
	else:
		valid_start = False
		not_video = False

	return not_none and not_empty and valid_start and not_video


def fix_url(current, url):
	'''Fix relative url paths.'''
	if current[0] == '/':
		return '{}{}'.format(get_domain(current), url)
	else:
		return current


def get_all_urls(url):
	'''Get all urls referenced by this url.'''
	urls = []

	print('requesting url')
	print(url)
	print(type(url))

	try:
		resp = requests.get(url)
	except requests.exceptions.InvalidSchema as e:
		print('>>>>>>>>>>>>>>>>>>>>')
		print(e)
		print('>>>>>>>>>>>>>>>>>>>>')
		return []
	except requests.exceptions.ConnectionError as e:
		print('>>>>>>>>>>>>>>>>>>>>')
		print(e)
		print('>>>>>>>>>>>>>>>>>>>>')
		return []
	except requests.exceptions.InvalidURL as e:
		print('>>>>>>>>>>>>>>>>>>>>')
		print(e)
		print('>>>>>>>>>>>>>>>>>>>>')
		return []
	except requests.exceptions.MissingSchema as e:
		print('>>>>>>>>>>>>>>>>>>>>')
		print(e)
		print('>>>>>>>>>>>>>>>>>>>>')
		return []

	soup = BeautifulSoup(resp.text, features='html.parser')
	for link in soup.find_all('a'):
		current = link.get('href')
		if is_valid(current):
			fixed = fix_url(current, url)
			urls.append(fixed)

	return urls


def init_result():
	'''Initialize the result with urls that have already been found.'''
	result = set()
	with open(OUTPUT_FILENAME, 'r') as f:
		for line in f.readlines():
			result.add(line.strip())
	return result


def update_counter(counter, result, searched, q):
	'''Update the current state of data in case of computer/network crash.'''
	if counter >= 100:
		with open(TEMP_FILENAME + 'result', 'w+') as f:
			f.write(str(result))

		with open(TEMP_FILENAME + 'searched', 'w+') as f:
			f.write(str(searched))

		with open(TEMP_FILENAME + 'q', 'w+') as f:
			temp = []
			while not q.empty():
				temp.append(q.get())
			f.write(str(temp))
			for elt in temp:
				q.put(elt)

		return 0
	else:
		return counter + 1


def retrieve_checkpoint():
	'''Get the last checkpoint data.'''
	q = queue.Queue()
	result = set()
	searched = set()

	print('CHECKPOINT VALUES ARE')

	with open(TEMP_FILENAME + 'q', 'r') as f:
		lines = f.readlines()
		if lines:
			print('loading last saved checkpoint (q)')
			temp = eval(lines[0])
			print('temp')
			print(temp)
			for elt in temp:
				q.put(elt)
		else:
			print('no checkpoint found')

	with open(TEMP_FILENAME + 'result', 'r') as f:
		lines = f.readlines()
		if lines:
			print('loading last saved checkpoint (result)')
			result = eval(lines[0])
			print(result)
		else:
			print('no checkpoint found')

	with open(TEMP_FILENAME + 'searched', 'r') as f:
		lines = f.readlines()
		if lines:
			print('loading last saved checkpoint (searched)')
			searched = eval(lines[0])
			print(searched)
		else:
			print('no checkpoint found')


	return q, result, searched


def main():
	'''Run main.'''
	num_urls = get_num_urls()
	q = queue.Queue()
	result = set()
	searched = set()

	q, result, searched = retrieve_checkpoint()

	q.put(START_URL)
	counter = 0

	while not q.empty() and len(result) < num_urls:

		counter = update_counter(counter, result, searched, q)

		print('starting iteration')
		next_url = q.get()
		if not is_valid(next_url):
			continue

		new_urls = get_all_urls(next_url)

		print('next_url')
		print(next_url)
		# print('new urls')
		# print(new_urls)

		for url in new_urls:
			if url not in searched:
				q.put(url)
				searched.add(url)
			if not is_wiki(url):
				result.add(get_domain((url)))

		print('result so far is')
		# pprint.pprint(result)
		print(len(result))
		print('searched {}', len(searched))

	with open(OUTPUT_FILENAME, 'w+') as f:
		for url in result:
			f.write('{}\n'.format(url))


main()