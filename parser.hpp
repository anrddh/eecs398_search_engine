#pragma once

#include <vector>
#include <deque>
#include <string>
#include <iostream>
#include <map>
#include "lib/stddef.hpp"

namespace fb
{
std::string extractURL( const std::string & line);
// // struct View {
// // 	 const char *p = nullptr;
// // 	 fb::SizeT s = 0;
// // };

// // std::ostream& operator<<(std::ostream &os, View &v) {
// // 	 for (fb::SizeT i = 0; i < v.s; ++i) {
// // 			os << v.p[i];
// // 	 }

// // 	 return os;
// // }

// // class ParsedUrl
// // 	{
// // 	public:
// // 		const std::string url;

// // 		View Service, Host, Port, Path;
// // 		 bool success = false;

// // 		ParsedUrl( std::string url_ ) : url(std::move(url_))
// // 			{
// // 			Service.p = url.c_str();
// // 			const char Colon = ':', Slash = '/';

// // 			fb::SizeT p = url.find(':');
// // 			if (p == std::string::npos || !p) {
// // 				Path.p = url.c_str();
// // 				Path.s = url.size();
// // 				success = true;
// // 				return;
// // 			}

// // 			Service.s = p++;

// // 			/*
// // 				TODO: If Service does not belong to allowed_services, we exit.
// // 			 */

// // 			if ( url[p] == Slash )
// // 				 p++;
// // 			if ( url[p] == Slash )
// // 				 p++;

// // 			Host.p = url.c_str() + p;

// // 			// check for stl algorithms
// // 			for ( ; p < url.size() && url[p] != Slash && url[p] != Colon; ++p )
// // 				 ;

// // 			Host.s = url.c_str() + p - Host.p;

// // 			if (p < url.size() && url[p] == Colon) {
// // 				// Port specified.  Skip over the colon and
// // 				// the port number.
// // 				 ++p;
// // 				 Port.p = url.c_str() + p;
// // 				 for (; p < url.size() && url[p] != Slash; ++p)
// // 						;
// // 				 Port.s = url.c_str() + p - Port.p;
// // 			}

// // 			Path.p = url.c_str() + p;
// // 			Path.s = url.size() - p;
// // 			success = true;
// // 		}

// // 		~ParsedUrl( )
// // 			{
// // 			delete[ ] pathBuffer;
// // 			}

// // 	private:
// // 		char *pathBuffer;
// // 		constexpr static std::array<const char *, 2> allowed_services = {"https", "http"};
// // 	};

// 	std::ostream& operator << ( std::ostream& os, ParsedUrl &testguy )
// 		{
// 		os << "complete: " << testguy.url << std::endl
// 			<< "service: " << testguy.Service << std::endl
// 			<< "host: " << testguy.Host << std::endl
// 			<< "port: " << testguy.Port << std::endl
// 			<< "path: " << testguy.Path << std::endl;
// 		return os;
// 		}

// Simple exception class for reporting String errors
struct ParserException {
	ParserException(const char* msg_) : msg(msg_)
		{}
	const char* msg;
};

bool isSpace( char c )
{
	return ( c == ' ' ) || ( c == '\t' ) || ( c == '\n' )
			|| ( c == '\v' ) || ( c == '\f' ) || ( c == '\r' );
}

class Parser
{
public:
	std::map<std::string, std::vector<std::string>> urlAnchorText;

	Parser( const std::string & content_in, const std::string & domain_in )
	: content( content_in ), domain( domain_in )
	{
		tagStack.push_back( "DEFAULT" );
	}

	std::string parse( )
		{
		std::string returnString;
		fb::SizeT index = 0;
		char lastChar = '0';
		try
		{
		while ( index < content.length( ) )
			{
			if ( content[ index ] == '<' )
				{
				index = handleTag( index );
				if ( !isSpace( lastChar ) )
					{
					returnString += " ";
					lastChar = ' ';
					}
				}
			else
				{
				if ( content[ index ] == '{' )
					index = seekSubstr(index, "}");
				else if ( content[ index ] == '}')
				{

				}
				else if ( !( isSpace( content[ index ] ) && isSpace( lastChar ) ) )
					{
					returnString += content[ index ];
					lastChar = content[ index ];
					}
				}
			++index;
			}
		}
		catch ( const ParserException & e)
		{
			std::cout << "Caught exception in " << domain << std::endl;
			std::cout << e.msg << std::endl;
		}
		return returnString;
		}

	void printUrls( ) const
		{
		for(auto i : urlAnchorText)
			{
			std::cout << "URL is: " << i.first << std::endl;
			std::cout << "Anchor text:" << std::endl;
			for(auto j : i.second)
				std::cout << "\t" << j << std::endl;
			}
		}

private:
	bool contentEqual( const fb::SizeT start, const std::string & rhs ) const
		{
		if ( start + rhs.length( ) > content.length( ) )
			throw ParserException(std::string("Comparison out of range " + std::to_string(start) + " " + rhs).c_str());

		for ( size_t i = 0;  i < rhs.length( );  ++i )
			{
			if ( content[ start + i ] != rhs[ i ] )
				return false;
			}
		return true;
		}

	bool contentEqualIgnoreCase( const fb::SizeT start, const std::string & rhs ) const
		{
		if ( start + rhs.length( ) > content.length( ) )
			throw ParserException(std::string("Comparison out of range " + std::to_string(start) + " " + rhs).c_str());

		for ( size_t i = 0;  i < rhs.length( );  ++i )
			{
			if ( tolower(content[ start + i ]) != tolower(rhs[ i ]) )
				return false;
			}
		return true;
		}

	// return the next index that is not a space
	fb::SizeT skipSpacesForward( fb::SizeT index ) const
		{
		while ( content[ index ] == ' ' )
			++index;
		return index;
		}

	// return the previous index that is not a space
	fb::SizeT skipSpacesBackward( fb::SizeT index ) const
		{
		while ( content[ index ] == ' ' )
			--index;
		return index;
		}

	// change tagStack appropriately 
	// opening tag or closing tag
	void setTag( std::string tagName )
	{
		if ( tagName[ 0 ] == '/' )
		{
			std::string tagType = tagName.substr( 1 );
			if ( tagStack.back( ) != tagType )
			{
				// std::cerr << "Tag do not match, something is wrong. Current tag: " 
				// 		<< tagStack.back() << " read: " << tagType <<  std::endl;
			}
			else
			 {
				if ( tagStack.empty() )
				{
					// std::cerr << "Possible tag error" << std::endl;
				}
				else
					tagStack.pop_back( );
			 }
		}
		else
			tagStack.push_back( tagName );
	}

	// look for occurence of str in content
	// and return the index of end of the occurence
	fb::SizeT seekSubstr( fb::SizeT index, const std::string& str ) const
	{
		while ( !contentEqual( index, str ) )
			++index;
		return index + str.length() - 1;
	}

	// look for occurence of str in content
	// and return the index of end of the occurence
	fb::SizeT seekSubstrIgnoreCase( fb::SizeT index, const std::string& str ) const
	{
		while ( !contentEqualIgnoreCase( index, str ) )
			++index;
		return index + str.length() - 1;
	}

	fb::SizeT handleQuote( fb::SizeT index ) const
		{
		bool inSingleQuote = false;
		bool inDoubleQuote = false;
		// handle escape character
		if( content[ index - 1 ] =='\\' )
			return index;

		while( index < content.length( ) )
			{
			if ( content[ index - 1 ] != '\\' )
				{
				char c = content[ index ];
				if( ( c == '\'' && inSingleQuote ) || ( c == '\"' && inDoubleQuote ) )
					break;

				// not in quote
				if( !inSingleQuote && !inDoubleQuote )
					{
					// enter single quote
					if( c == '\'' )
						inSingleQuote = true;
					// enter double quote
					else if( c == '\"' )
						inDoubleQuote = true;
					}
				}
				++index;
			}
			return index;
		}

	// handle tag and return the end index, which is index of ">".
	// start_index is the index of "<"
	fb::SizeT handleTag( fb::SizeT start_index )
		{
			fb::SizeT i = start_index + 1;

			i = skipSpacesForward( i );

			if ( contentEqual( i, "!-" ) )
				return seekSubstr( i , "-->" );

			std::string tagName;
			for ( ;  content[ i ] != ' ' && content[ i ] != '>' && i < content.length( );  ++i )
				tagName += tolower( content[ i ] );

			// std::cerr << tagName << " " << content[i] << std::endl;
			// std::cerr << i << " length " << content.length() << std::endl;

			// i = seekSubstr( i, ">" );
			while ( content[i] != '>' )
			{
				if ( content[i] == '\"' || content[i] == '\'')
					i = handleQuote(i);
				++i;
			}

			// std::cerr << "after seek " << i << " " << content[i] << std::endl;

			size_t last_index = skipSpacesBackward( i - 1 );
			
			// not self closing tag
			if ( content[ last_index ] != '/')
				{
				if ( tagName == "script" )
					i = handleScript( i + 1 );
				else if ( tagName == "a" )
					i = handleAnchor( start_index, i + 1 );
				else if ( tagName == "style" )
					i = handleStyle( start_index );
				else
					setTag( tagName );
				}
			// std::cerr << "end handle tag" << std::endl;
			return i;
		}

	fb::SizeT handleStyle( fb::SizeT index ) const
	{
		index = seekSubstrIgnoreCase(index, "</style");
		index = seekSubstr(index, ">");
		return index;
	}


	// Given the start and end indices open anchor tag
	// figure out the link in the tag
	// and get the anchor text, and add those to urlAnchorText
	// return index of ">" of closing anchor tag
	fb::SizeT handleAnchor( fb::SizeT tagStartIndex, fb::SizeT tagEndIndex )
	{
		fb::SizeT index = tagEndIndex;

		std::string aTag = content.substr( tagStartIndex, tagEndIndex - tagStartIndex );

		index = seekSubstr( index, "<" );

		std::string url = extractURL( aTag );
		std::string anchorText = content.substr( tagEndIndex, index - tagEndIndex );

		// while ( !contentEqual( index, "</a" ) && !contentEqual( index, "</A" ) )
		// 	++index;

		index = seekSubstrIgnoreCase( index, "</a" );

		if( url[0] != '#' )
		{
			if( url[0] == '/' )
				url = domain + url;

			urlAnchorText[ url ].push_back( anchorText );
		}

		index = seekSubstr( index, ">" );

		return index;
	}


	// skip comment block in java script and return index at the end
	fb::SizeT skipJSCommentBlock( fb::SizeT index ) const
		{
		while ( !contentEqual(index, "*/") )
			{
			if ( contentEqual(index, "<![CDATA"))
			{
				index = seekSubstr( index, "]]>" );
				break;
			}
			++index;
			}
		index = seekSubstr( index, "*/" );
		return index + 1;
		}

	// skip comment in java script and return index at the end
	fb::SizeT skipJSCommentLine( fb::SizeT index, bool & endScript ) const
		{
		// anything between these interpreted literally.
		// if ( contentEqual(index, "//<![CDATA["))
		// 	return seekSubstr(index, "//]]>");

		while( index < content.length() )
			{
			if ( contentEqual(index, "<![CDATA"))
				index = seekSubstr( index, "]]>" );

			if ( contentEqual( index, "\n" ) )
			{
				endScript = false;
				return index;
			}
			if ( contentEqualIgnoreCase( index, "</script" ) )
			{
				endScript = true;
				index = seekSubstr( index, ">");
				return index;
			}
			++index;
			}
		endScript = true;
		return content.length();
		}

	// return the index of ">" of end of the closing tag of a script
	fb::SizeT handleScript( fb::SizeT index ) const
		{
		// std::cerr << "enter script " << index << " " << content.substr(index - 10, 30) << std::endl;
		bool inSingleQuote = false;
		bool inDoubleQuote = false;
		while( index < content.length( ) )
			{
			char c = content[ index ];

			// skip quote
			if ( c == '\'' || c == '\"')
				{
				index = handleQuote( index );
				}
			else
				{
				// skip line comment
				if( c == '/' && content[ index + 1 ] == '/' )
					{
					bool endScript = false;
					index = skipJSCommentLine( index, endScript );
					if ( endScript )
						break;
					}
				// skip comment block
				else if ( c == '/' && content[ index + 1 ] == '*' )
					index = skipJSCommentBlock( index );
				}

			if( contentEqualIgnoreCase( index, "</script" ))
				{
				index = seekSubstr( index, ">" );
				break;
				}

			++index;
			}
			return index;
		}

	// domain name of html page being parsed
	const std::string domain;
	// content of the html page to parse
	const std::string & content;
	// stack to contain the tags
	std::vector<std::string> tagStack;
};

std::string extractURL( const std::string & line )
	{
	fb::SizeT found = line.find( "href" );
	if ( found == std::string::npos )
		found = line.find( "HREF" );
	// There might be space between href and =
	found = line.find( "\"", found );
	if ( found == std::string::npos )
		return "";
	// found + 1 to find the next closing quote
	// if no closing quote is found, skip.
	fb::SizeT endURL = line.find( "\"", found + 1 );
	if( endURL == std::string::npos )
		return "";

	std::string url = line.substr( found + 1, endURL - found - 1);
	// just in case a closing quote is not really closing the url, one heuristic is
	// that there would be a space somewhere.
	// For example, we might have
	// href="https://www.nytimes.com/es/ href =    "https://www.nytimes.com/es/
	// it mighbe be possible to just add the substring until the space, but we will see.
	if( url.find( " " ) == std::string::npos )
		return url;
	else
		return "";
}

// take in string and a container, vector for now, and append newly found urls
// to the given container.
void extractURL( const std::string & line, std::deque< std::string > & urls , std::string domain)
	 {
	 fb::SizeT found = line.find( "href" );
	 while( found != std::string::npos )
			{
			// There might be space between href and =
			found = line.find( "\"", found );
			if ( found == std::string::npos )
				return;
			// found + 1 to find the next closing quote
			// if no closing quote is found, skip.
			fb::SizeT endURL = line.find( "\"", found + 1 );
			if( endURL == std::string::npos )
				return;

			std::string url = line.substr( found + 1, endURL - found - 1);

			// just in case a closing quote is not really closing the url, one heuristic is
			// that there would be a space somewhere.
			// For example, we might have
			// href="https://www.nytimes.com/es/ href =    "https://www.nytimes.com/es/
			// it mighbe be possible to just add the substring until the space, but we will see.
			if( url.find( " " ) == std::string::npos )
				{
				// ParsedUrl parsedUrl( url.c_str( ) );
				if( url.length() )
					if(url[0] == '/')
					{
					 url = domain + url;
					}
				urls.push_back( url );
				}
			found = line.find( "href", endURL );
			}
	 }
};
