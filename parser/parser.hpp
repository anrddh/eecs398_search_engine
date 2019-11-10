#pragma once

#include <vector>
#include <deque>
#include <string>
#include <iostream>
#include <map>
#include "../lib/stddef.hpp"

namespace fb
{
std::string extractURL( const std::string & line);
struct View {
	 const char *p = nullptr;
	 fb::SizeT s = 0;
};

std::ostream& operator<<(std::ostream &os, View &v) {
	 for (fb::SizeT i = 0; i < v.s; ++i) {
			os << v.p[i];
	 }

	 return os;
}

class ParsedUrl
	{
	public:
		const std::string url;

		View Service, Host, Port, Path;
		 bool success = false;

		ParsedUrl( std::string url_ ) : url(std::move(url_))
			{
			Service.p = url.c_str();
			const char Colon = ':', Slash = '/';

			fb::SizeT p = url.find(':');
			if (p == std::string::npos || !p) {
				Path.p = url.c_str();
				Path.s = url.size();
				success = true;
				return;
			}

			Service.s = p++;

			
				// TODO: If Service does not belong to allowed_services, we exit.
			 

			if ( url[p] == Slash )
				 p++;
			if ( url[p] == Slash )
				 p++;

			Host.p = url.c_str() + p;

			// check for stl algorithms
			for ( ; p < url.size() && url[p] != Slash && url[p] != Colon; ++p )
				 ;

			Host.s = url.c_str() + p - Host.p;

			if (p < url.size() && url[p] == Colon) {
				// Port specified.  Skip over the colon and
				// the port number.
				 ++p;
				 Port.p = url.c_str() + p;
				 for (; p < url.size() && url[p] != Slash; ++p)
						;
				 Port.s = url.c_str() + p - Port.p;
			}

			Path.p = url.c_str() + p;
			Path.s = url.size() - p;
			success = true;
		}

		~ParsedUrl( )
			{
			delete[ ] pathBuffer;
			}

	private:
		char *pathBuffer;
		// constexpr static std::array<const char *, 2> allowed_services = {"https", "http"};
	};

	std::ostream& operator << ( std::ostream& os, ParsedUrl &testguy )
		{
		os << "complete: " << testguy.url << std::endl
			<< "service: " << testguy.Service << std::endl
			<< "host: " << testguy.Host << std::endl
			<< "port: " << testguy.Port << std::endl
			<< "path: " << testguy.Path << std::endl;
		return os;
		}

// Simple exception class for reporting String errors
struct ParserException 
{
	ParserException(const std::string msg_) : msg(msg_)
		{
		}

	const std::string msg;
};

bool isSpace( char c )
	{
	return ( c == ' ' ) || ( c == '\t' ) || ( c == '\n' )
			|| ( c == '\v' ) || ( c == '\f' ) || ( c == '\r' );
	}

class Parser
{
public:
	const static std::map<std::string, std::string> characterConversionMap;
	std::map<std::string, std::string> urlAnchorText;
	bool inSpecialCharacter;
	std::string specialCharacterString;
	char lastChar;

	std::vector<std::string> parsedWords;

	Parser( const std::string &content_in, const std::string &domain_in )
	: content( content_in ), domain( domain_in ), inSpecialCharacter( false ),
		specialCharacterString( "" ), lastChar( '0' )
	{
		tagStack.push_back( "DEFAULT" );
		parsedWords.push_back( "" );
	}

	void convertParsedResult( )
	{
		for( auto i : parsedWords )
			parsedResult += i + " ";
	}

	std::string getParsedResult( )
		{
			convertParsedResult();
		return parsedResult;
		}

	std::string getSpecialCharacter( )
	{
		auto iter = characterConversionMap.find( specialCharacterString );
		if ( iter == characterConversionMap.end( ) )
			return " ";
		else
			return iter->second;
	}

	void addWord( std::string str )
	{
		for ( auto i : str )
			addWord( i );
	}

	void addWord( char c )
	{
		if ( isSpace( c ) || ispunct( c ) )
		{
			if ( parsedWords.back( ) != "" )
				parsedWords.push_back("");
		}
		else
		{
			parsedWords.back() += c;
		}
	}

	void addToResult( char c )
	{
		if( c == '&' )
			inSpecialCharacter = true;
		else if ( inSpecialCharacter )
		{
			if ( c == ';' )
			{
				inSpecialCharacter = false;
				// parsedResult += getSpecialCharacter( );
				addWord( getSpecialCharacter( ) );
				specialCharacterString = "";
			}
			else
			{
				specialCharacterString += c;
			}
		}
		else
		{
			if ( !( isSpace( c ) && isSpace( lastChar ) ) )
			{
				if ( isSpace( c ) )
					c = ' ';
				// parsedResult += c;
				addWord( c );
				lastChar = c;
			}
		}

	}

	void parse( )
		{
		parsedResult.clear( );
		parsedResult.reserve( content.length( ) );

		fb::SizeT index = 0;
		try
			{
			while ( index < content.length( ) )
				{
				if ( content[ index ] == '<' )
					{
					index = handleTag( index );
					addToResult( ' ' );
					}
				else
					{
					if ( content[ index ] == '{' )
						index = seekSubstr(index, "}");
					else if ( content[ index ] == '}')
					{

					}
					// else if ( !( isSpace( content[ index ] ) && isSpace( lastChar ) ) )
					// 	{
					// 	parsedResult += content[ index ];
					// 	lastChar = content[ index ];
					// 	}
					addToResult( content[ index ] );
					}
				++index;
				}
			}
		catch ( const ParserException & e )
			{
			std::cerr << "Caught exception in " << domain << std::endl;
			std::cerr << e.msg << std::endl;
			}
		}

	void printUrls( ) const
		{
		for(auto i : urlAnchorText)
			{
			std::cout << "URL is: " << i.first << std::endl;
			std::cout << "Anchor text: " << i.second << std::endl;
			}
		}

private:

	std::string contentEqualErrorMsg( const fb::SizeT start, const std::string &rhs ) const
		{
		std::string errorMsg = "Comparison out of range.\n";
		errorMsg += "Start Index: " + std::to_string( start ) + "\n";
		errorMsg += "Content Length: " + std::to_string( content.length( ) ) + "\n";
		errorMsg += "RHS: " + rhs + "\n";
		errorMsg += "Content string: " + content.substr( start ) + "\n";
		return errorMsg;
		}
	// check if content[start : start + rhs.length()] == rhs
	// throw exception if try to access out of range
	bool contentEqual( const fb::SizeT start, const std::string &rhs ) const
		{
		if ( start + rhs.length( ) > content.length( ) )
			throw ParserException( contentEqualErrorMsg( start, rhs ) );

		for ( size_t i = 0;  i < rhs.length( );  ++i )
			{
			if ( content[ start + i ] != rhs[ i ] )
				return false;
			}
		return true;
		}

	// check if content[start : start + rhs.length()] == rhs with case ignored
	// throw exception if try to access out of range
	bool contentEqualIgnoreCase( const fb::SizeT start, const std::string & rhs ) const
		{
		if ( start + rhs.length( ) > content.length( ) )
			throw ParserException( contentEqualErrorMsg( start, rhs ) );

		for ( size_t i = 0;  i < rhs.length( );  ++i )
			{
			if ( tolower( content[ start + i ] ) != tolower( rhs[ i ] ) )
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
			if ( tagStack.back ( ) == tagType )
				{
				if ( !tagStack.empty( ) )
					tagStack.pop_back( );
				// else
				// 	std::cerr << "Possible tag error" << std::endl;
				}
			// else
				// std::cerr << "Tag do not match, something is wrong. Current tag: " 
				// 		<< tagStack.back() << " read: " << tagType <<  std::endl;
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
		return index + str.length( ) - 1;
	}

	// look for occurence of str in content
	// and return the index of end of the occurence
	fb::SizeT seekSubstrIgnoreCase( fb::SizeT index, const std::string& str ) const
	{
		while ( !contentEqualIgnoreCase( index, str ) )
			++index;
		return index + str.length( ) - 1;
	}

	fb::SizeT seekUnescaped( fb::SizeT index, const std::string& str ) const
	{
		while ( !contentEqual( index, str ) )
		{
			if ( content[ index ] == '\\' )
				index += 2;
			else
				++index;
		}
		return index + str.length( ) - 1;
	}

	// given index that starts at a quotation mark
	// return index of end of quotation
	fb::SizeT handleQuote( fb::SizeT index ) const
		{
		// handle escape character
		if( content[ index - 1 ] =='\\' )
			return index;

		if ( content[ index ] == '\'' )
			return seekUnescaped( index, "\'" );

		if ( content[ index ] == '\"' )
			return seekUnescaped( index, "\"" );

		throw ParserException("Index given is not an index of quotation mark");
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

			while ( content[ i ] != '>' && i < content.length( ) )
				{
				if ( content[ i ] == '\"' || content[i] == '\'')
					i = handleQuote(i);
				++i;
				}

			size_t last_index = skipSpacesBackward( i - 1 );
			
			// not self closing tag
			if ( content[ last_index ] != '/' )
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
			return i;
		}

	fb::SizeT handleStyle( fb::SizeT index ) const
		{
		index = seekSubstrIgnoreCase( index, "</style" );
		index = seekSubstr( index, ">" );
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
		
		// add anchor text to parsed result
		addToResult( ' ' );
		for ( auto i : anchorText )
			addToResult( i );
		addToResult( ' ' );

		// if( !parsedResult.empty( ) && !isSpace( parsedResult.back( ) ) )
		// 	parsedResult += " ";
		// parsedResult += anchorText;
		// if( !parsedResult.empty( ) && !isSpace( parsedResult.back( ) ) )
		// 	parsedResult += " ";

		index = seekSubstrIgnoreCase( index, "</a" );

		if ( url.length( ) )
			{
			if( url[ 0 ] != '#' )
				{
				if( url[ 0 ] == '/' )
					url = domain + url;

				urlAnchorText[ url ] += " " + anchorText;
				}
			}

		index = seekSubstr( index, ">" );

		return index;
		}


	// skip comment block in java script and return index at the end
	fb::SizeT skipJSCommentBlock( fb::SizeT index ) const
		{
		while ( !contentEqual( index, "*/" ) )
			{
			if ( contentEqual( index, "<![CDATA" ) )
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
	fb::SizeT skipJSCommentLine( fb::SizeT index, bool &endScript ) const
		{
		while( index < content.length() )
			{
			// this is literally the only hope in html & javascript
			if ( contentEqual(index, "<![CDATA"))
				index = seekSubstr( index, "]]>" );

			if ( contentEqual( index, "\n" ) )
				{
				endScript = false;
				return index;
				}

			// apparnetly end of script tag finishes comment
			if ( contentEqualIgnoreCase( index, "</script" ) )
				{
				endScript = true;
				index = seekSubstr( index, ">" );
				return index;
				}

			++index;
			}

		endScript = true;
		return content.length( );
		}

	// return the index of ">" of end of the closing tag of a script
	fb::SizeT handleScript( fb::SizeT index ) const
		{
		bool inSingleQuote = false;
		bool inDoubleQuote = false;
		while( index < content.length( ) )
			{
			char c = content[ index ];

			// skip quote
			if ( c == '\'' || c == '\"')
				index = handleQuote( index );
			else
				{
				// skip line comment
				if ( contentEqual( index, "//" ) )
					{
					bool endScript = false;
					index = skipJSCommentLine( index, endScript );
					if ( endScript )
						break;
					}
				// skip comment block
				else if ( contentEqual( index, "/*" ) )
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

	std::string parsedResult;

	// stack to contain the tags
	std::vector<std::string> tagStack;
};

const std::map<std::string, std::string> Parser::characterConversionMap = 
{
	{ "#192", "A" },
	{ "#193", "A" },
	{ "#194", "A" },
	{ "#195", "A" },
	{ "#196", "A" },
	{ "#197", "A" },

	{ "Agrave", "A" },
	{ "Aacute", "A" },
	{ "Acirc", "A" },
	{ "Atilde", "A" },
	{ "Auml", "A" },
	{ "Aring", "A" },

	{ "#198", "AE" },
	{ "AElig", "AE" },

	{ "#199", "C" },
	{ "Ccedil", "C" },

	{ "#200", "E" },
	{ "#201", "E" },
	{ "#202", "E" },
	{ "#203", "E" },

	{ "Egrave", "E" },
	{ "Eacute", "E" },
	{ "Ecirc", "E" },
	{ "Euml", "E" },

	{ "#204", "I" },
	{ "#205", "I" },
	{ "#206", "I" },
	{ "#207", "I" },

	{ "Igrave", "I" },
	{ "Iacute", "I" },
	{ "Icirc", "I" },
	{ "Iuml", "I" },

	{ "#209", "N" },
	{ "Ntilde", "N" },

	{ "#210", "O" },
	{ "#211", "O" },
	{ "#212", "O" },
	{ "#213", "O" },
	{ "#214", "O" },
	{ "#216", "O" },

	{ "Ograve", "O" },
	{ "Oacute", "O" },
	{ "Ocirc", "O" },
	{ "Otilde", "O" },
	{ "Ouml", "O" },
	{ "Oslash", "O" },

	{ "#217", "U" },
	{ "#218", "U" },
	{ "#219", "U" },
	{ "#220", "U" },

	{ "Ugrave", "U" },
	{ "Uacute", "U" },
	{ "Ucirc", "U" },
	{ "Uuml", "U" },

	{ "#221", "Y" },
	{ "Yacute", "Y" },

	{ "#224", "a" },
	{ "#225", "a" },
	{ "#226", "a" },
	{ "#227", "a" },
	{ "#228", "a" },
	{ "#229", "a" },

	{ "agrave", "a" },
	{ "aacute", "a" },
	{ "acirc", "a" },
	{ "atilde", "a" },
	{ "auml", "a" },
	{ "aring", "a" },

	{ "#230", "ae" },
	{ "aelig", "ae" },

	{ "#231", "c" },
	{ "ccedil", "c" },

	{ "#232", "e" },
	{ "#233", "e" },
	{ "#234", "e" },
	{ "#235", "e" },

	{ "egrave", "e" },
	{ "eacute", "e" },
	{ "ecirc", "e" },
	{ "euml", "e" },

	{ "#236", "i" },
	{ "#237", "i" },
	{ "#238", "i" },
	{ "#239", "i" },

	{ "igrave", "i" },
	{ "iacute", "i" },
	{ "icirc", "i" },
	{ "iuml", "i" },

	{ "#241", "n" },
	{ "ntilde", "n" },

	{ "#242", "o" },
	{ "#243", "o" },
	{ "#244", "o" },
	{ "#245", "o" },
	{ "#246", "o" },
	{ "#248", "o" },

	{ "ograve", "o" },
	{ "oacute", "o" },
	{ "ocirc", "o" },
	{ "otilde", "o" },
	{ "ouml", "o" },
	{ "oslash", "o" },

	{ "#249", "u" },
	{ "#250", "u" },
	{ "#251", "u" },
	{ "#252", "u" },

	{ "ugrave", "u" },
	{ "uacute", "u" },
	{ "ucirc", "u" },
	{ "uuml", "u" },

	{ "#253", "y" },
	{ "yacute", "y" },
	{ "#255", "y" },
	{ "yuml", "y" },
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
