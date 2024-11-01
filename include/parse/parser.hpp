#pragma once

#include <iostream>

#include <fb/string.hpp>
#include <fb/no_delete_unordered_map.hpp>
#include <fb/unordered_set.hpp>
#include <fb/stddef.hpp>
#include <disk/page_store.hpp>
#include <http/download_html.hpp>

#include <disk/logfile.hpp>
#include <debug.hpp>

#include <ctype.h>

// #include "../../index/index_builder.hpp"
//flags
constexpr uint8_t INDEX_WORD_TITLE = 0b0100;
constexpr uint8_t INDEX_WORD_BOLD = 0b1000;
constexpr uint8_t INDEX_WORD_HEADER = 0b0010;
constexpr uint8_t INDEX_WORD_ANCHOR = 0b0001;

// Simple exception class for reporting String errors
struct ParserException
{
	ParserException(const fb::String msg_) : msg(msg_)
		{
		}

	const fb::String msg;
};

class Parser
{
public:
	// fb::NoDeleteUnorderedMap<fb::String, fb::String> urlAnchorText;
	// fb::Vector<fb::String> urls;
	fb::UnorderedSet<fb::String> urls;
	fb::Vector<uint8_t> wordFlags;
	const ParsedUrl parsedUrl;
    constexpr static bool addUrls = false;

	Parser( fb::StringView content_in, const ParsedUrl parsedUrl_in )
        : parsedUrl(parsedUrl_in), content(content_in), inSpecialCharacter(false)
		{
		tagStack.pushBack( "DEFAULT" );

		for ( int i = 0; i < 4; ++i )
			flagCounter[i] = 0;
		}

   // This function will invalidate the iarser object
   // Written by Jaeyoon Kim
   Page extractPage( fb::SizeT UrlOffset )
      {
      Page p;
      p.UrlOffset = UrlOffset;
      p.page_str = std::move( parsedResult );
      p.word_headers = std::move( wordFlags );
      return p;
      }

    fb::String getParsedResult( ) {
		return std::move(parsedResult);
    }

	void parse( )
		{
		parsedResult.clear( );
		parsedResult.reserve( content.size( ) );
		parsedResult += ' ';

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
			// std::cerr << "Caught exception in " << parsedUrl.CompleteUrl << std::endl;
			// std::cerr << e.msg << std::endl;
			}
		}

	void printUrls( )
		{
		// for ( auto i = urlAnchorText.begin();   i != urlAnchorText.end();  ++i )
		// 	{
  //               log(logfile, "URL is: ", i.key(), '\n',
  //                   "Anchor text: ", *i, '\n');
		// 	}
		for ( auto i = urls.begin( );  i != urls.end( );  ++i ) 
			log(logfile, "URL is: ", *i, '\n');
		}

private:
    void addWord( const fb::String &str )
		{
		for ( const auto i : str )
			addWord( i );
		}

	void addWord( const char c )
		{
		if ( isspace( c ) || ispunct( c ) || !isalnum( c ) )
			{
			if ( parsedResult.back( ) != ' ' )
				{
				parsedResult += ' ';
				wordFlags.pushBack( getWordFlag( ) );
				}
			}
		else if ( isalnum(c) )
			parsedResult += c;
		}

	void addToResult( const char c )
		{
		if( c == '&' )
			inSpecialCharacter = true;
		else if ( inSpecialCharacter )
			{
			if ( c == ';' )
				{
				inSpecialCharacter = false;
				}
			}
		else
			addWord( c );
		}

    fb::StringView extractURL(const fb::SizeT start,
                              const fb::SizeT end ) {
    	fb::StringView tagView = content.substr( start, end - start );

		fb::SizeT found = tagView.find( "href"_sv );
		if ( found == fb::StringView::npos )
			found = tagView.find( "HREF"_sv );
		// There might be space between href and =
		found = tagView.find( "\""_sv, found );
		if ( found == fb::StringView::npos )
			return {};
		// found + 1 to find the next closing quote
		// if no closing quote is found, skip.
		fb::SizeT endURL = tagView.find( "\""_sv, found + 1 );
		if ( endURL == fb::StringView::npos )
			return {};

		fb::StringView url = tagView.substr( found + 1, endURL - found - 1 );
		// just in case a closing quote is not really closing the url, one heuristic is
		// that there would be a space somewhere.
		// For example, we might have
		// href="https://www.nytimes.com/es/ href =    "https://www.nytimes.com/es/
		// it mighbe be possible to just add the substring until the space, but we will see.
        return url.find(' ') != fb::StringView::npos ? ""_sv : url;
    }

    fb::String contentEqualErrorMsg( ) const {
        fb::String errorMsg = "Comparison out of range.\n";
		return errorMsg;
    }

	// check if content[start : start + rhs.size()] == rhs
	// throw exception if try to access out of range
	bool contentEqual(const fb::SizeT start,
                      fb::StringView rhs ) const {
        if ( start + rhs.size( ) > content.size( ) )
            throw ParserException( contentEqualErrorMsg( ) );
        for ( size_t i = 0;  i < rhs.size( );  ++i )
            if ( content[ start + i ] != rhs[ i ] )
                return false;

        return true;
    }

	// check if content[start : start + rhs.size()] == rhs with case ignored
	// throw exception if try to access out of range
	bool contentEqualIgnoreCase(const fb::SizeT start,
                                fb::StringView rhs ) const
		{
		if ( start + rhs.size( ) > content.size( ) )
			throw ParserException( contentEqualErrorMsg( ) );

		for ( fb::SizeT i = 0;  i < rhs.size( );  ++i )
			if ( tolower( content[ start + i ] ) != tolower( rhs[ i ] ) )
				return false;

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

	fb::UnorderedSet<fb::String> italicTags;
	int flagCounter[4];

	uint8_t getWordFlag( )
		{
		uint8_t result = 0;
		if ( flagCounter[ 0 ] > 0 )
			result += INDEX_WORD_TITLE;
		if ( flagCounter[ 1 ] > 0 )
			result += INDEX_WORD_BOLD;
		if ( flagCounter[ 2 ] > 0 )
			result += INDEX_WORD_HEADER;
		return result;
		}

	void setFlagCounter( fb::StringView tagType, int change )
		{
		if ( tagType == "title"_sv )
			flagCounter[0] += change;
		else if ( boldTags.find( tagType ) != boldTags.end( ) )
			flagCounter[1] += change;
		else if ( tagType.size( ) == 2 && tagType.front() == 'h' )
			flagCounter[2] += change;
		}

	// change tagStack appropriately
	// opening tag or closing tag
	void setTag( fb::String tagName )
		{
        if ( tagName.front() == '/' )
			{
                tagName = tagName.substr(1);
			if ( !tagStack.empty() && tagStack.back( ) == tagName )
				{
				tagStack.popBack( );
				setFlagCounter( tagName, -1 );
				}
			}
		else
			{
			setFlagCounter(tagName, 1);
			tagStack.pushBack(std::move(tagName));
			}
		}

	// look for occurence of str in content
	// and return the index of end of the occurence
	fb::SizeT seekSubstr(fb::SizeT index,
                         fb::StringView str) const
		{
		while ( !contentEqual( index, str ) )
			++index;
		return index + str.size( ) - 1;
		}

	// look for occurence of str in content
	// and return the index of end of the occurence
	fb::SizeT seekSubstrIgnoreCase(fb::SizeT index,
                                   fb::StringView str) const
		{
		while ( !contentEqualIgnoreCase( index, str ) )
			++index;
		return index + str.size( ) - 1;
		}

	fb::SizeT seekUnescaped( fb::SizeT index, fb::StringView str ) const
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
			return seekUnescaped( index, "\'"_sv );

		if ( content[ index ] == '\"' )
			return seekUnescaped( index, "\""_sv );

		throw ParserException("Index given is not an index of quotation mark");
		}

	// handle tag and return the end index, which is index of ">".
	// start_index is the index of "<"
	fb::SizeT handleTag( fb::SizeT start_index )
		{
		fb::SizeT i = start_index + 1;

		i = skipSpacesForward( i );

		if ( contentEqual( i, "!-"_sv ) )
			return seekSubstr( i , "-->"_sv );

		fb::String tagName;
		for ( ;  content[ i ] != ' '
				&& content[ i ] != '>' && i < content.size( );  ++i )
			tagName += tolower( content[ i ] );

		while ( content[ i ] != '>' && i < content.size( ) )
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
			else if ( tagName == "html" )
				handleHTML( start_index, last_index );
			else
				setTag( std::move(tagName) );
			}

		return i;
		}

	void handleHTML( fb::SizeT start, fb::SizeT end ) const
		{
        auto htmlView = content.substr( start, end - start );
		fb::String htmlLower;
        htmlLower.resize(htmlView.size());
        //(htmlView.data(), htmlView.size());
		// there are so many insane people online. lower case everything
		// to keep myself sane
		for ( fb::SizeT i = 0;  i < htmlLower.size( );  ++i )
			htmlLower[ i ] = tolower( htmlView[ i ] );
		//fb::StringView htmlTag( htmlLower.data( ), htmlLower.size( ) );
		fb::SizeT index = htmlLower.find( "lang"_sv );
		if ( index != fb::String::npos )
			{
			if ( htmlLower.find( "en"_sv, index ) == fb::String::npos
				&& htmlLower.find( "mul"_sv, index ) == fb::String::npos )
				throw ParserException( "language not english" );
			}
		}

	fb::SizeT handleStyle( fb::SizeT index ) const
		{
		index = seekSubstrIgnoreCase( index, "</style"_sv );
		index = seekSubstr( index, ">"_sv );
		return index;
		}

	fb::StringView trimSpace( const fb::StringView str )
		{
		fb::SizeT start = 0;
		while(  start < str.size( ) && str[ start ] == ' ' )
			++start;

		// if the string is just all space
		if ( start == str.size( ) )
			return  "";

		//  string is not all space, so end should never reach 0
		fb::SizeT end = str.size( ) - 1;
		while( str[ end ] == ' ' )
			--end;

		return str.substr( start, end - start + 1 );
		}

	void addUrlAnchorTest( fb::String url )
		// , fb::String normalizedText )
		{
		if ( url.empty( ) )
			return;

		if( url[ 0 ] == '/' )
			{
			if( url[ 1 ] == '/' )
				url = parsedUrl.Service +  ":" + url;
			else
				url = parsedUrl.Service + "://" + parsedUrl.Host + url;
			}
		else if ( url[ 0 ] == '.' )
			url = parsedUrl.Service + "://" + parsedUrl.Host + "/" + url;

		urls.insert( url );
		// if ( !normalizedText.empty( ) )
		// 	{
		// 	auto normalizedView = trimSpace(normalizedText);

		// 	fb::String & anchorText = urlAnchorText[ url ];
		// 	if (!anchorText.empty())
		// 		anchorText += ' ';
		// 	anchorText += normalizedView;
		// 	}
		}

	bool isActualUrl( fb::StringView url )
		{
		return url.startsWith( "http://" ) || url.startsWith( "https://" )
			|| url.startsWith( '.' ) || url.startsWith( '/' );
		}

	// Given the start and end indices open anchor tag
	// figure out the link in the tag
	// and get the anchor text, and add those to urlAnchorText
	// return index of ">" of closing anchor tag
	fb::SizeT handleAnchor( fb::SizeT tagStartIndex, fb::SizeT tagEndIndex )
		{
		fb::SizeT index = tagEndIndex;
		//fb::String aTag = content.substr( tagStartIndex, tagEndIndex - tagStartIndex );

        index = seekSubstr( index, "<"_sv );

        fb::StringView anchorText = content.substr(tagEndIndex,
               index - tagEndIndex );

		addToResult( ' ' );

		for ( char i : anchorText )
			addToResult( i );
		addToResult( ' ' );

        if constexpr ( addUrls )
	        {
	        fb::StringView urlView = extractURL( tagStartIndex, tagEndIndex );
	        fb::String url(urlView.data(), urlView.size());
			if ( isActualUrl( url ) )
				addUrlAnchorTest( std::move(url) );
			}

		index = seekSubstrIgnoreCase( index, "</a"_sv );
		index = seekSubstr( index, ">"_sv );

		return index;
		}


	// skip comment block in java script and return index at the end
	fb::SizeT skipJSCommentBlock( fb::SizeT index ) const
		{
		while ( !contentEqual( index, "*/" ) )
			{
			if ( contentEqual( index, "<![CDATA" ) )
			{
				index = seekSubstr( index, "]]>"_sv );
				break;
			}
			++index;
			}
		index = seekSubstr( index, "*/"_sv );
		return index + 1;
		}

	// skip comment in java script and return index at the end
	fb::SizeT skipJSCommentLine( fb::SizeT index, bool &endScript ) const
		{
		while( index < content.size() )
			{
			// this is literally the only hope in html & javascript
			if ( contentEqual( index, "<![CDATA") )
				index = seekSubstr( index, "]]>"_sv );

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

			if( contentEqualIgnoreCase( index, "</script" ) )
				{
				index = seekSubstr( index, ">" );
				break;
				}

			++index;
			}
			return index;
		}

	// stack to contain the tags
	fb::Vector<fb::String> tagStack;

	// content of the html page to parse
    fb::StringView content;

    fb::String parsedResult;

	bool inSpecialCharacter;

    struct InitParser;
    friend class InitParser;
    struct InitParser {
        InitParser() {
            Parser::init();
        }
    };

    static InitParser p;
	static fb::UnorderedSet<fb::StringView> boldTags;

    static void init() {
		initializeBoldTags( );
    }

	static void initializeBoldTags() {
		boldTags.insert( "b"_sv ); // bold
		boldTags.insert( "strong"_sv ); // bold
		boldTags.insert( "u"_sv ); // underline
		boldTags.insert( "mark"_sv ); // highlight
		boldTags.insert( "i"_sv ); // italic
		boldTags.insert( "em"_sv ); // italic
    }
};
