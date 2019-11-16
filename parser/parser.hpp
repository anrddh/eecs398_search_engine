#pragma once

#include "../lib/string.hpp"
#include <iostream>
#include "../lib/unordered_map.hpp"
#include "../lib/stddef.hpp"

namespace fb
{
// Simple exception class for reporting String errors
struct ParserException 
{
	ParserException(const String msg_) : msg(msg_)
		{
		}

	const String msg;
};

bool isSpace( char c )
	{
	return ( c == ' ' ) || ( c == '\t' ) || ( c == '\n' )
			|| ( c == '\v' ) || ( c == '\f' ) || ( c == '\r' );
	}

class Parser
{
public:
	fb::UnorderedMap<String, String> characterConversionMap;
	fb::UnorderedMap<String, String> urlAnchorText;
	bool inSpecialCharacter;
	String specialCharacterString;
	char lastChar;

	fb::Vector<String> parsedWords;

	Parser( const String &content_in, const String &domain_in )
	: content( content_in ), domain( domain_in ), inSpecialCharacter( false ),
		specialCharacterString( "" ), lastChar( '0' )
	{
		initializerConversionMap( );
		tagStack.pushBack( "DEFAULT" );
		parsedWords.pushBack( "" );
	}

	void convertParsedResult( )
	{
		for( auto i : parsedWords )
		{
			// std::cout << i << std::endl;
			parsedResult += i + " ";
		}
	}

	String getParsedResult( )
		{
		convertParsedResult();
		return parsedResult;
		}

	String getSpecialCharacter( )
	{
		try 
		{
			return characterConversionMap.at( specialCharacterString );
		}
		catch ( ... )
		{
			return "";
		}

		// auto iter = characterConversionMap.find( specialCharacterString );
		// if ( iter == characterConversionMap.end( ) )
		// 	return " ";
		// else
		// 	return iter->second;
	}

	void addWord( String str )
	{
		for ( auto i : str )
			addWord( i );
	}

	void addWord( char c )
	{
		if ( isSpace( c ) || ispunct( c ) || !isalnum( c ) )
		{
			if( !parsedWords.back( ).empty( ) )
				parsedWords.pushBack("");
		}
		else if ( isalnum(c) )
		{
			parsedWords.back( ) += c;
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
				addWord( c );
				lastChar = c;
			}
		}

	}

	void parse( )
		{
		parsedResult.clear( );
		parsedResult.reserve( content.size( ) );

		fb::SizeT index = 0;
		try
			{
			while ( index < content.size( ) )
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

	void printUrls( )
		{
		for( auto i = urlAnchorText.begin(); i != urlAnchorText.end(); ++i )
			{
			std::cout << "URL is: " << i.key() << std::endl;
			std::cout << "Anchor text: " << *i << std::endl;
			}
		}

private:

	// exclusive end
	fb::SizeT find( const fb::SizeT start, const fb::SizeT end, const String &rhs )
		{
		for( fb::SizeT i = start;  i + rhs.size( ) <= end;  ++i )
			if ( contentEqual( i, rhs ) )
				return i;

		return end;
		}

	String extractURL( const fb::SizeT start, const fb::SizeT end )
		{
		fb::SizeT found = find( start, end, "href" );
		if ( found == end )
			found = find( start, end, "HREF" );
		// There might be space between href and =
		found = find( found, end, "\"" );
		if ( found == end )
			return "";
		// found + 1 to find the next closing quote
		// if no closing quote is found, skip.
		fb::SizeT endURL = find( found + 1, end, "\"" );
		if( endURL == end )
			return "";

		String url = content.substr( found + 1, endURL - found - 1 );
		// just in case a closing quote is not really closing the url, one heuristic is
		// that there would be a space somewhere.
		// For example, we might have
		// href="https://www.nytimes.com/es/ href =    "https://www.nytimes.com/es/
		// it mighbe be possible to just add the substring until the space, but we will see.
		for ( int i = 0;  i < url.size( );  ++i )
			if( url[ i ] == ' ' )
				return "";

		return url;
		}

	String contentEqualErrorMsg( const fb::SizeT start, const String &rhs ) const
		{
		String errorMsg = "Comparison out of range.\n";
		// errorMsg.append( "Start Index: " + std::to_string( start ) + "\n" );
		// errorMsg.append( "Content Length: " + std::to_string( content.size( ) ) + "\n" );
		// errorMsg.append( "RHS: " + rhs + "\n" );
		// errorMsg.append( "Content string: " + content.substr( start ) + "\n" );
		return errorMsg;
		}
	// check if content[start : start + rhs.size()] == rhs
	// throw exception if try to access out of range
	bool contentEqual( const fb::SizeT start, const String &rhs ) const
		{
		if ( start + rhs.size( ) > content.size( ) )
			throw ParserException( contentEqualErrorMsg( start, rhs ) );

		for ( size_t i = 0;  i < rhs.size( );  ++i )
			{
			if ( content[ start + i ] != rhs[ i ] )
				return false;
			}
		return true;
		}

	// check if content[start : start + rhs.size()] == rhs with case ignored
	// throw exception if try to access out of range
	bool contentEqualIgnoreCase( const fb::SizeT start, const String & rhs ) const
		{
		if ( start + rhs.size( ) > content.size( ) )
			throw ParserException( contentEqualErrorMsg( start, rhs ) );

		for ( size_t i = 0;  i < rhs.size( );  ++i )
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
	void setTag( const String tagName )
	{
		// std::cout << "in set Tag" << std::endl;
		if ( tagName[ 0 ] == '/' )
			{
			String tagType = tagName.substr( 1 );
			if ( tagStack.back( ) == tagType )
				{
				if ( !tagStack.empty( ) )
					tagStack.popBack( );
				// else
				// 	std::cerr << "Possible tag error" << std::endl;
				}
			// else
				// std::cerr << "Tag do not match, something is wrong. Current tag: " 
				// 		<< tagStack.back() << " read: " << tagType <<  std::endl;
			}
		else
		{
			String tagType = tagName;
			tagStack.pushBack( tagType );
		}
	}

	// look for occurence of str in content
	// and return the index of end of the occurence
	fb::SizeT seekSubstr( fb::SizeT index, const String& str ) const
	{
		while ( !contentEqual( index, str ) )
			++index;
		return index + str.size( ) - 1;
	}

	// look for occurence of str in content
	// and return the index of end of the occurence
	fb::SizeT seekSubstrIgnoreCase( fb::SizeT index, const String& str ) const
	{
		while ( !contentEqualIgnoreCase( index, str ) )
			++index;
		return index + str.size( ) - 1;
	}

	fb::SizeT seekUnescaped( fb::SizeT index, const String& str ) const
	{
		while ( !contentEqual( index, str ) )
		{
			if ( content[ index ] == '\\' )
				index += 2;
			else
				++index;
		}
		return index + str.size( ) - 1;
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

			String tagName;
			for ( ;  content[ i ] != ' ' && content[ i ] != '>' && i < content.size( );  ++i )
				tagName += tolower( content[ i ] );

			while ( content[ i ] != '>' && i < content.size( ) )
				{
				if ( content[ i ] == '\"' || content[i] == '\'')
					i = handleQuote(i);
				++i;
				}

			size_t last_index = skipSpacesBackward( i - 1 );
			
			// std::cout << "before " << tagName << " " << tagName.size() << std::endl;
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
			// std::cout << tagName << " " << tagName.size() << std::endl;
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

		String aTag = content.substr( tagStartIndex, tagEndIndex - tagStartIndex );

		index = seekSubstr( index, "<" );

		String url = extractURL( tagStartIndex, tagEndIndex );
		String anchorText = content.substr( tagEndIndex, index - tagEndIndex );
		
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

		if ( url.size( ) )
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
		while( index < content.size() )
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
		return content.size( );
		}

	// return the index of ">" of end of the closing tag of a script
	fb::SizeT handleScript( fb::SizeT index ) const
		{
		bool inSingleQuote = false;
		bool inDoubleQuote = false;
		while( index < content.size( ) )
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


void initializerConversionMap()
{
	characterConversionMap[ "#192"] = "A";
	characterConversionMap[ "#193"] = "A";
	characterConversionMap[ "#194"] = "A";
	characterConversionMap[ "#195"] = "A";
	characterConversionMap[ "#196"] = "A";
	characterConversionMap[ "#197"] = "A";

	characterConversionMap[ "Agrave"] = "A";
	characterConversionMap[ "Aacute"] = "A";
	characterConversionMap[ "Acirc"] = "A";
	characterConversionMap[ "Atilde"] = "A";
	characterConversionMap[ "Auml"] = "A";
	characterConversionMap[ "Aring"] = "A";

	characterConversionMap[ "#198"] = "AE";
	characterConversionMap[ "AElig"] = "AE";

	characterConversionMap[ "#199"] = "C";
	characterConversionMap[ "Ccedil"] = "C";

	characterConversionMap[ "#200"] = "E";
	characterConversionMap[ "#201"] = "E";
	characterConversionMap[ "#202"] = "E";
	characterConversionMap[ "#203"] = "E";

	characterConversionMap[ "Egrave"] = "E";
	characterConversionMap[ "Eacute"] = "E";
	characterConversionMap[ "Ecirc"] = "E";
	characterConversionMap[ "Euml"] = "E";

	characterConversionMap[ "#204"] = "I";
	characterConversionMap[ "#205"] = "I";
	characterConversionMap[ "#206"] = "I";
	characterConversionMap[ "#207"] = "I";

	characterConversionMap[ "Igrave"] = "I";
	characterConversionMap[ "Iacute"] = "I";
	characterConversionMap[ "Icirc"] = "I";
	characterConversionMap[ "Iuml"] = "I";

	characterConversionMap[ "#209"] = "N";
	characterConversionMap[ "Ntilde"] = "N";

	characterConversionMap[ "#210"] = "O";
	characterConversionMap[ "#211"] = "O";
	characterConversionMap[ "#212"] = "O";
	characterConversionMap[ "#213"] = "O";
	characterConversionMap[ "#214"] = "O";
	characterConversionMap[ "#216"] = "O";

	characterConversionMap[ "Ograve"] = "O";
	characterConversionMap[ "Oacute"] = "O";
	characterConversionMap[ "Ocirc"] = "O";
	characterConversionMap[ "Otilde"] = "O";
	characterConversionMap[ "Ouml"] = "O";
	characterConversionMap[ "Oslash"] = "O";

	characterConversionMap[ "#217"] = "U";
	characterConversionMap[ "#218"] = "U";
	characterConversionMap[ "#219"] = "U";
	characterConversionMap[ "#220"] = "U";

	characterConversionMap[ "Ugrave"] = "U";
	characterConversionMap[ "Uacute"] = "U";
	characterConversionMap[ "Ucirc"] = "U";
	characterConversionMap[ "Uuml"] = "U";

	characterConversionMap[ "#221"] = "Y";
	characterConversionMap[ "Yacute"] = "Y";

	characterConversionMap[ "#224"] = "a";
	characterConversionMap[ "#225"] = "a";
	characterConversionMap[ "#226"] = "a";
	characterConversionMap[ "#227"] = "a";
	characterConversionMap[ "#228"] = "a";
	characterConversionMap[ "#229"] = "a";

	characterConversionMap[ "agrave"] = "a";
	characterConversionMap[ "aacute"] = "a";
	characterConversionMap[ "acirc"] = "a";
	characterConversionMap[ "atilde"] = "a";
	characterConversionMap[ "auml"] = "a";
	characterConversionMap[ "aring"] = "a";

	characterConversionMap[ "#230"] = "ae";
	characterConversionMap[ "aelig"] = "ae";

	characterConversionMap[ "#231"] = "c";
	characterConversionMap[ "ccedil"] = "c";

	characterConversionMap[ "#232"] = "e";
	characterConversionMap[ "#233"] = "e";
	characterConversionMap[ "#234"] = "e";
	characterConversionMap[ "#235"] = "e";

	characterConversionMap[ "egrave"] = "e";
	characterConversionMap[ "eacute"] = "e";
	characterConversionMap[ "ecirc"] = "e";
	characterConversionMap[ "euml"] = "e";

	characterConversionMap[ "#236"] = "i";
	characterConversionMap[ "#237"] = "i";
	characterConversionMap[ "#238"] = "i";
	characterConversionMap[ "#239"] = "i";

	characterConversionMap[ "igrave"] = "i";
	characterConversionMap[ "iacute"] = "i";
	characterConversionMap[ "icirc"] = "i";
	characterConversionMap[ "iuml"] = "i";

	characterConversionMap[ "#241"] = "n";
	characterConversionMap[ "ntilde"] = "n";

	characterConversionMap[ "#242"] = "o";
	characterConversionMap[ "#243"] = "o";
	characterConversionMap[ "#244"] = "o";
	characterConversionMap[ "#245"] = "o";
	characterConversionMap[ "#246"] = "o";
	characterConversionMap[ "#248"] = "o";

	characterConversionMap[ "ograve"] = "o";
	characterConversionMap[ "oacute"] = "o";
	characterConversionMap[ "ocirc"] = "o";
	characterConversionMap[ "otilde"] = "o";
	characterConversionMap[ "ouml"] = "o";
	characterConversionMap[ "oslash"] = "o";

	characterConversionMap[ "#249"] = "u";
	characterConversionMap[ "#250"] = "u";
	characterConversionMap[ "#251"] = "u";
	characterConversionMap[ "#252"] = "u";

	characterConversionMap[ "ugrave"] = "u";
	characterConversionMap[ "uacute"] = "u";
	characterConversionMap[ "ucirc"] = "u";
	characterConversionMap[ "uuml"] = "u";

	characterConversionMap[ "#253"] = "y";
	characterConversionMap[ "yacute"] = "y";
	characterConversionMap[ "#255"] = "y";
	characterConversionMap[ "yuml"] = "y";
}

	// domain name of html page being parsed
	const String domain;
	// content of the html page to parse
	const String & content;

	String parsedResult;

	// stack to contain the tags
	fb::Vector<String> tagStack;
};

// const fb::UnorderedMap<String, String> Parser::characterConversionMap = 
// {
// 	{ "#192", "A" },
// 	{ "#193", "A" },
// 	{ "#194", "A" },
// 	{ "#195", "A" },
// 	{ "#196", "A" },
// 	{ "#197", "A" },

// 	{ "Agrave", "A" },
// 	{ "Aacute", "A" },
// 	{ "Acirc", "A" },
// 	{ "Atilde", "A" },
// 	{ "Auml", "A" },
// 	{ "Aring", "A" },

// 	{ "#198", "AE" },
// 	{ "AElig", "AE" },

// 	{ "#199", "C" },
// 	{ "Ccedil", "C" },

// 	{ "#200", "E" },
// 	{ "#201", "E" },
// 	{ "#202", "E" },
// 	{ "#203", "E" },

// 	{ "Egrave", "E" },
// 	{ "Eacute", "E" },
// 	{ "Ecirc", "E" },
// 	{ "Euml", "E" },

// 	{ "#204", "I" },
// 	{ "#205", "I" },
// 	{ "#206", "I" },
// 	{ "#207", "I" },

// 	{ "Igrave", "I" },
// 	{ "Iacute", "I" },
// 	{ "Icirc", "I" },
// 	{ "Iuml", "I" },

// 	{ "#209", "N" },
// 	{ "Ntilde", "N" },

// 	{ "#210", "O" },
// 	{ "#211", "O" },
// 	{ "#212", "O" },
// 	{ "#213", "O" },
// 	{ "#214", "O" },
// 	{ "#216", "O" },

// 	{ "Ograve", "O" },
// 	{ "Oacute", "O" },
// 	{ "Ocirc", "O" },
// 	{ "Otilde", "O" },
// 	{ "Ouml", "O" },
// 	{ "Oslash", "O" },

// 	{ "#217", "U" },
// 	{ "#218", "U" },
// 	{ "#219", "U" },
// 	{ "#220", "U" },

// 	{ "Ugrave", "U" },
// 	{ "Uacute", "U" },
// 	{ "Ucirc", "U" },
// 	{ "Uuml", "U" },

// 	{ "#221", "Y" },
// 	{ "Yacute", "Y" },

// 	{ "#224", "a" },
// 	{ "#225", "a" },
// 	{ "#226", "a" },
// 	{ "#227", "a" },
// 	{ "#228", "a" },
// 	{ "#229", "a" },

// 	{ "agrave", "a" },
// 	{ "aacute", "a" },
// 	{ "acirc", "a" },
// 	{ "atilde", "a" },
// 	{ "auml", "a" },
// 	{ "aring", "a" },

// 	{ "#230", "ae" },
// 	{ "aelig", "ae" },

// 	{ "#231", "c" },
// 	{ "ccedil", "c" },

// 	{ "#232", "e" },
// 	{ "#233", "e" },
// 	{ "#234", "e" },
// 	{ "#235", "e" },

// 	{ "egrave", "e" },
// 	{ "eacute", "e" },
// 	{ "ecirc", "e" },
// 	{ "euml", "e" },

// 	{ "#236", "i" },
// 	{ "#237", "i" },
// 	{ "#238", "i" },
// 	{ "#239", "i" },

// 	{ "igrave", "i" },
// 	{ "iacute", "i" },
// 	{ "icirc", "i" },
// 	{ "iuml", "i" },

// 	{ "#241", "n" },
// 	{ "ntilde", "n" },

// 	{ "#242", "o" },
// 	{ "#243", "o" },
// 	{ "#244", "o" },
// 	{ "#245", "o" },
// 	{ "#246", "o" },
// 	{ "#248", "o" },

// 	{ "ograve", "o" },
// 	{ "oacute", "o" },
// 	{ "ocirc", "o" },
// 	{ "otilde", "o" },
// 	{ "ouml", "o" },
// 	{ "oslash", "o" },

// 	{ "#249", "u" },
// 	{ "#250", "u" },
// 	{ "#251", "u" },
// 	{ "#252", "u" },

// 	{ "ugrave", "u" },
// 	{ "uacute", "u" },
// 	{ "ucirc", "u" },
// 	{ "uuml", "u" },

// 	{ "#253", "y" },
// 	{ "yacute", "y" },
// 	{ "#255", "y" },
// 	{ "yuml", "y" },
// }; 

};
