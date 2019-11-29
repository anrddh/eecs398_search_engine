#include "fb/string_list.hpp"

#include "bolt/html_page.hpp"

#include <fstream>
#include <iostream>
#include <sstream>

class HtmlTemplate 
   {
public:
   void loadTemplateFromFile(fb::String path) 
      {
      std::ifstream file;
      file.open(path.data());
      file.seekg(0, std::ios::end);
      html.resize(file.tellg());
      file.seekg(0, std::ios::beg);
      file.read(&html[0], html.size());
      file.close();
      }

   void loadTemplateFromString(const fb::String &templ) { html = templ; }

   fb::String getRenderedHtml(fb::UnorderedMap<fb::String, fb::String>& variables) 
      {
      fb::StringList sl;
      for (htmlIter = html.begin(); htmlIter < html.end() - 1; ++htmlIter) 
         {
         if (*htmlIter == '[' && *(htmlIter + 1) == '%') 
            {
            loadVariableIntoTemplate(variables, sl);
            } 
         else 
            {
            sl << *htmlIter;
            }
         }
      sl << *htmlIter;

      return sl.str();
      }

private:
  fb::String html;
  fb::String::Iterator htmlIter;

  void loadVariableIntoTemplate(fb::UnorderedMap<fb::String, fb::String>& variables,
      fb::StringList& sl) 
      {
      ++htmlIter;
      while (*(++htmlIter) == ' ')
         ;

      auto stringStart = htmlIter;
      while (*(++htmlIter) != ' ')
         ;

      fb::String variableKey(stringStart, htmlIter);
      if (variables.find(variableKey) != variables.end()) 
         {
         sl << variables.at(variableKey);
         }

      while (((*htmlIter != '%') || (*(htmlIter + 1) != ']')) &&
           htmlIter < html.end() - 1) 
         {
         ++htmlIter;
      }
      ++htmlIter;
  }
};

HtmlPage::HtmlPage() : htmlTemplate(fb::makeUnique<HtmlTemplate>()) {}

HtmlPage::HtmlPage(const HtmlPage& other)
   : header(other.header),
     templateVariables(other.templateVariables),
     htmlTemplate(fb::makeUnique<HtmlTemplate>(*other.htmlTemplate))
   { }

HtmlPage& HtmlPage::operator=(const HtmlPage& other) 
   {
   if (this != &other) 
      {
      htmlTemplate = fb::makeUnique<HtmlTemplate>(*other.htmlTemplate);
      header = other.header;
      templateVariables = other.templateVariables;
      }
   return *this;
   }

HtmlPage ::~HtmlPage() {}

void HtmlPage::loadFromString(const fb::String &html) 
   {
   htmlTemplate->loadTemplateFromString(html);
   }
void HtmlPage::loadFromFile(const fb::String &path) 
   {
   htmlTemplate->loadTemplateFromFile(path);
   }

void HtmlPage::setValue(const fb::String &key, const fb::String &value) 
   {
   templateVariables[key] = value;
   }

fb::String HtmlPage::getPageHtml() 
   {
   return htmlTemplate->getRenderedHtml(templateVariables);
   }
