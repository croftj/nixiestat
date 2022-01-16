# include "menu.h"
# include "core/HardwareSerial.h"
# include <avr/pgmspace.h>
# include <ctype.h>
# include <stdio.h>
# include <stdlib.h>
# include "WProgram.h"

extern char *p_buf;
extern char *ibuf;

static bool dbg = 0;
static bool dbg1 = 0;
static char pbuf[80];
static char tbuf[80];
static char entry[80];

MenuEntry::MenuEntry(const char *name, MenuType_t type, void* value, uint8_t val_size, MenuEntry *parent) :
    m_name(name),
    m_type(type),
    m_valueSize(val_size),
    m_value(value),
    m_action(NULL),
    m_input(NULL),
    m_output(NULL),
    m_imin(0),
    m_imax(0),
    m_umin(0),
    m_umax(0),
    m_parent(parent),
    m_1stChild(NULL),
    m_nextSibling(NULL)
{
//   Serial << __PRETTY_FUNCTION__ << strcpy_P(pbuf, PSTR(" Constructor")) << endl;
   if ( parent != NULL ) 
   {
      parent->AddChildMenu(this);
   }
}

MenuEntry::MenuEntry(const char *name, MenuType_t type, int* value, MenuEntry *parent, int min, int max) :
    m_name(name),
    m_type(type),
    m_valueSize(0),
    m_value(value),
    m_action(NULL),
    m_input(NULL),
    m_output(NULL),
    m_imin(min),
    m_imax(max),
    m_umin(0),
    m_umax(0),
    m_parent(parent),
    m_1stChild(NULL),
    m_nextSibling(NULL)
{
//   Serial << __PRETTY_FUNCTION__ << strcpy_P(pbuf, PSTR(" Constructor")) << endl;
   if ( parent != NULL ) 
   {
      parent->AddChildMenu(this);
   }
}

MenuEntry::MenuEntry(const char *name, MenuType_t type, unsigned int* value, MenuEntry *parent, unsigned int min, unsigned int max) :
    m_name(name),
    m_type(type),
    m_valueSize(0),
    m_value(value),
    m_action(NULL),
    m_input(NULL),
    m_output(NULL),
    m_imin(0),
    m_imax(0),
    m_umin(min),
    m_umax(max),
    m_parent(parent),
    m_1stChild(NULL),
    m_nextSibling(NULL)
{
//   Serial << __PRETTY_FUNCTION__ << strcpy_P(pbuf, PSTR(" Constructor")) << endl;
   if ( parent != NULL ) 
   {
      dbg1 = 0;
      parent->AddChildMenu(this);
   }
}

MenuEntry::MenuEntry(const char *name, MenuType_t type, float* value, MenuEntry *parent, float min, float max) :
    m_name(name),
    m_type(type),
    m_valueSize(0),
    m_value(value),
    m_action(NULL),
    m_input(NULL),
    m_output(NULL),
    m_fmin(0),
    m_fmax(0),
    m_umin(min),
    m_umax(max),
    m_parent(parent),
    m_1stChild(NULL),
    m_nextSibling(NULL)
{
//   Serial << __PRETTY_FUNCTION__ << strcpy_P(pbuf, PSTR(" Constructor")) << endl;
   if ( parent != NULL ) 
   {
      parent->AddChildMenu(this);
   }
}

MenuEntry::MenuEntry(const char *name, MenuType_t type, bool (*input)(const char*), void (*output)(), MenuEntry *parent) :
    m_name(name),
    m_type(type),
    m_valueSize(0),
    m_value(NULL),
    m_action(NULL),
    m_input(input),
    m_output(output),
    m_imin(0),
    m_imax(0),
    m_umin(0),
    m_umax(0),
    m_parent(parent),
    m_1stChild(NULL),
    m_nextSibling(NULL)
{
//   Serial << __PRETTY_FUNCTION__ << strcpy_P(pbuf, PSTR(" Constructor")) << endl;
//   Serial << strcpy_P(pbuf, PSTR("- m_input = ")) << (uint32_t)m_input << endl;
//   Serial << strcpy_P(pbuf, PSTR("- m_output = ")) << (uint32_t)m_output << endl;
   if ( parent != NULL ) 
   {
      parent->AddChildMenu(this);
   }
}

MenuEntry::MenuEntry(const char *name, MenuType_t type, bool (*action)(), MenuEntry *parent) :
    m_name(name),
    m_type(type),
    m_valueSize(0),
    m_value(NULL),
    m_action(action),
    m_input(NULL),
    m_output(NULL),
    m_imin(0),
    m_imax(0),
    m_umin(0),
    m_umax(0),
    m_parent(parent),
    m_1stChild(NULL),
    m_nextSibling(NULL)
{
//   Serial << __PRETTY_FUNCTION__ << strcpy_P(pbuf, PSTR(" Constructor"));
//   Serial << strcpy_P(pbuf, PSTR("- m_action = ")) << (uint32_t)m_action << endl;
   if ( parent != NULL ) 
   {
      parent->AddChildMenu(this);
   }
}

MenuEntry::MenuEntry(const char *name, MenuType_t type, MenuEntry *parent) :
    m_name(name),
    m_type(type),
    m_valueSize(0),
    m_value(0),
    m_action(NULL),
    m_input(NULL),
    m_output(NULL),
    m_imin(0),
    m_imax(0),
    m_umin(0),
    m_umax(0),
    m_parent(parent),
    m_1stChild(NULL),
    m_nextSibling(NULL)
{
//   Serial << __PRETTY_FUNCTION__ << strcpy_P(pbuf, PSTR(" Constructor")) << endl;
   if ( parent != NULL ) 
   {
      parent->AddChildMenu(this);
   }
}

void MenuEntry::AddChildMenu(MenuEntry *child)
{
   child->m_nextSibling = m_1stChild;
   m_1stChild = child;
}

void MenuEntry::PrintPath()
{
   MenuEntry *menu_names[64];
   int8_t menu_depth = 0;

   if ( m_parent == NULL ) 
   {
      menu_names[menu_depth] = this;
      menu_depth++;
   }
   else 
   {
      for (MenuEntry *p = this; p != NULL; p = p->m_parent ) 
      {
         if ( p->m_parent != NULL ) 
         {
            menu_names[menu_depth] = p;
            menu_depth++;
         }
      }
   }

   while (menu_depth > 0) 
   {
      Serial << strcpy_P(pbuf, menu_names[--menu_depth]->m_name);
      if ( menu_depth > 0 ) 
      {
         Serial << strcpy_P(pbuf, PSTR("."));
      }
   }
}

bool MenuEntry::PrintEntry()
{
   bool rv = true;
//   Serial << "Calculating max length" << endl;
   if ( m_type == MainMenu || m_type == SubMenu ) 
   {
      Serial << endl; // Serial.print(strcpy_P(pbuf, PSTR("\r\n")));
      Serial << strcpy_P(pbuf, PSTR(" **** ["));
      PrintPath();
      Serial << strcpy_P(pbuf, PSTR("] ****"));
      Serial << strcpy_P(pbuf, PSTR("\r\n"));
      uint8_t entry_len = 0;
      for (MenuEntry *p = m_1stChild; p != NULL; p = p->m_nextSibling) 
      {
//         Serial << strcpy_P(pbuf, PSTR(" 0x"));
//         Serial << itoa((long)p, ibuf, 16);
//         Serial << strcpy_P(pbuf, PSTR(", "));
         memset(tbuf, '\x00', sizeof(tbuf));
         strncpy_P(tbuf, p->m_name, sizeof(tbuf) - 1);
         uint8_t len = strlen(tbuf);
///         Serial << "entry: " << tbuf << ", len " << (int)len << ", entry_len = " << (int)entry_len << endl;
         if ( len > entry_len ) 
         {
//            Serial << "Found new entry_len" << endl;
            entry_len = len;
         }
      }
      Serial << strcpy_P(pbuf, PSTR("\r\n"));

      MenuEntry *p = m_1stChild;
      char key[2] = "`";
      for (uint8_t entry_cnt = 0; rv != false && p != NULL; p = p->m_nextSibling) 
      {
         (*key)++;
         Serial << key;
         Serial << strcpy_P(pbuf, PSTR(") "));
         Serial << strncpy_P(pbuf, p->m_name, 64);
         if ( p->m_type != Action && p->m_type != MainMenu && p->m_type != SubMenu ) 
         {
            uint8_t targ_len = entry_len + 5 - strlen_P(p->m_name);
            for (uint8_t x = 0; x < targ_len; x++) 
            {
               Serial << strcpy_P(pbuf, PSTR("."));
            }
            Serial << strcpy_P(pbuf, PSTR(" ["));
            p->PrintValue();
            Serial << strcpy_P(pbuf, PSTR("]\r\n"));
         }
         else 
         {
            Serial << strcpy_P(pbuf, PSTR("\r\n"));
         }
      }
   }
   else
   {
      Serial << strcpy_P(pbuf, m_name);
      if ( m_type != Action && m_type != MainMenu && m_type != SubMenu)
      {
         Serial << strcpy_P(pbuf, PSTR(" ["));
         PrintValue();
         Serial << strcpy_P(pbuf, PSTR("]"));
      }
   }
}

bool MenuEntry::PrintValue()
{
   char outbuf[80];
   memset(outbuf, '\x00', sizeof(outbuf));
   switch (m_type) 
   {
      case CustomIO:
         if ( m_output != NULL ) 
         {
            (*m_output)();
            break;
         }

      case Float:
         sprintf(outbuf, "%f", *(double*)m_value);
//         Serial << *(double*)m_value;
         break;

      case Int8:
         sprintf(outbuf, "%d", *(int8_t*)m_value);
//         Serial << *(int8_t*)m_value;
         break;

      case Int16:
         sprintf(outbuf, "%d", *(int16_t*)m_value);
//         Serial << *(int16_t*)m_value;
         break;

      case Int32:
         sprintf(outbuf, "%d", *(int32_t*)m_value);
//         Serial << *(int32_t*)m_value;
         break;

      case String:
         strncpy(outbuf, (char*)m_value, strlen(outbuf) - 1);
//         Serial << *(char*)m_value;
         break;

      case TimeMin:
         {
            if ( *(int16_t*)m_value == 0xffff ) 
            {
               strcpy_P(outbuf, PSTR("Not Set"));
            }
            else 
            {
               uint16_t hr = *(int16_t*)m_value / 60;
               uint16_t min = *(int16_t*)m_value % 60;
//               sprintf(time, strcpy_P(pbuf, PSTR("(0x%04x:%d) %02d:%02d")), *(int16_t*)m_value, *(int16_t*)m_value, hr, min);
               sprintf(outbuf, strcpy_P(pbuf, PSTR("%02d:%02d")), hr, min);
            }
         }
         break;

      case UInt8:
         sprintf(outbuf, "%u", *(uint8_t*)m_value);
//         Serial << (int)(*(uint8_t*)m_value);
         break;

      case UInt16:
         sprintf(outbuf, "%u", *(uint16_t*)m_value);
//         Serial << (int)(*(uint16_t*)m_value);
         break;

      case UInt32:
         sprintf(outbuf, "%u", *(uint32_t*)m_value);
//         Serial << (int)(*(uint32_t*)m_value);
         break;
         
      case YesNo:
         if ( *(bool *)m_value == true ) 
         {
            strcpy_P(outbuf, PSTR("Yes"));
         }
         else 
         {
            strcpy_P(outbuf, PSTR("no"));
//            Serial << strcpy_P(pbuf, PSTR("No"));
         }

      case Action:
      case MainMenu:
      case SubMenu:
         break;
   }
   Serial << outbuf;
   return(true);
}

MenuEntry *MenuEntry::Execute(char *buf, MenuEntry *first_menu, bool &change_made)
{
   MenuEntry *rv = first_menu;

   memset(entry, '\x00', sizeof(entry));
   if ( strlen(buf) > 0 ) 
   {
      if ( m_type == MainMenu ||
            m_type == SubMenu ) 
      {
         char ch = 'a';
         char *cp = entry;
         while (ch >= '\x20' && ch != '?' && ch != '.' && ch != '=' ) 
         {
            if ( strlen(entry) < sizeof(entry) - 1 ) 
            {
               ch = *(buf++);
               if ( ch >= '\x20' && ch != '?' && ch != '.' && ch != '=' && ch  ) 
               {
                  *(cp++) = tolower(ch);
               }
            }
         }
         if (dbg1) Serial << strcpy_P(pbuf, PSTR("entry = '")) << entry;
         if (dbg1) Serial << strcpy_P(pbuf, PSTR("'")) << endl;
         if ( strlen(entry) > 0 ) 
         {
            MenuEntry *it = FindChild(entry);
            if ( it != NULL ) 
            {
//               if (dbg) Serial << __PRETTY_FUNCTION__ << "found menu = '" << strcpy_P(tbuf, it->m_name) << "'" << endl;
//               if (dbg) Serial << __PRETTY_FUNCTION__ << "ch = " << (int)ch << endl;
               if ( ch == '.' || ch == '=' ) 
               {
//                  if ( it->m_type == MainMenu ||
//                        it->m_type == SubMenu ) 
//                  {
//                     if (dbg) Serial << __PRETTY_FUNCTION__ << "Calling exec for = '" << strcpy_P(tbuf, it->m_name) << "', buf = '" << buf << "'\r\n";
                     rv = it->Execute(buf, first_menu, change_made);
//                  }
               }
               else if ( ch == '?')
               {
                  Serial << endl;
                  Serial << "=";
                  it->PrintValue();
                  Serial << " @ ";
                  it->PrintPath();
                  Serial << endl;
                  change_made = false;
               }
               else if ( isspace(ch) || ch == '\0')
               {
                  /***********************************************/
                  /*   If  we  are  at  an Action entry, do it,  */
                  /*   otherwise just return the pointer to the  */
                  /*   entry                                     */
                  /***********************************************/
                  if ( it->m_type == Action )
                  {
                     if ( it->m_action != NULL ) 
                     {
                        (*it->m_action)();
                     }
                     else 
                     {
                        Serial << strcpy_P(pbuf, PSTR("Invalid Action defined\r\n"));
                     }
                  }
                  else 
                  {
//                     if (dbg) Serial << strcpy_P(pbuf, PSTR("Setting rv to entry: ")) << strcpy_P(tbuf, it->m_name) << endl;
                     rv = it;
                  }
               }
               else
               {
                  Serial << strcpy_P(pbuf, PSTR("Error- Unexpected character after '"));
                  Serial << entry;
                  Serial << strcpy_P(pbuf, PSTR("'\r\n"));
               }
            }
            else 
            {
//               Serial << (int)(SP);
               Serial << strcpy_P(pbuf, PSTR("\r\nAck!!!\r\n"));
//               Serial << entry;
            }
         }
      }
      else 
      {
//         if (dbg) Serial << __PRETTY_FUNCTION__ << "single entry, buf = " << buf;
//         if (dbg) Serial << strcpy_P(pbuf, PSTR("\r\n"));
         double fv;
         int32_t iv;
         uint32_t uv;
//         if (dbg) Serial << __PRETTY_FUNCTION__ << "m_type = '" << (int)m_type << endl;
         switch (m_type) 
         {
            case CustomIO:
               if ( m_input != NULL ) 
               {
                  change_made = (*m_input)(buf);
                  break;
               }

            case Action:
               change_made = (*m_action)();
               break;

            case Float:
               fv = atof(buf);
//               if (dbg) Serial << __PRETTY_FUNCTION__ << "Float = " << fv;
//               if (dbg) Serial << strcpy_P(pbuf, PSTR("'\r\n"));
               if ( m_fmin == m_fmax ||
                     (fv >= m_fmin && fv <= m_fmax))
               {
                  *((double*)m_value) = fv;
                  change_made = true;
               }
               break;

            case Int8:
               iv = strtol(buf, NULL, 0);
//               if (dbg) Serial << __PRETTY_FUNCTION__ << "Int8 = " << (int)iv;
//               if (dbg) Serial << strcpy_P(pbuf, PSTR("'\r\n"));
               if ( iv < 128 && iv > -127 &&
                     (m_imin == m_imax ||
                     (iv >= m_imin && iv <= m_imax ))) 
               {
                  *((int8_t*)m_value) = iv;
                  change_made = true;
               }
               break;

            case Int16:
               iv = strtol(buf, NULL, 0);
//               if (dbg) Serial << __PRETTY_FUNCTION__ << "Int16 = " << (int)iv;
//               if (dbg) Serial << strcpy_P(pbuf, PSTR("'\r\n"));
               if ( iv < 32767 && iv > -32766 &&
                     (m_imin == m_imax ||
                     (iv >= m_imin && iv <= m_imax ))) 
               {
                  *((int16_t*)m_value) = iv;
                  change_made = true;
               }
               break;

            case Int32:
               iv = strtol(buf, NULL, 0);
//               if (dbg) Serial << __PRETTY_FUNCTION__ << "Int32 = " << (int)iv;
//               if (dbg) Serial << strcpy_P(pbuf, PSTR("'\r\n"));
               if ( m_imin == m_imax ||
                     (iv >= m_imin && iv <= m_imax ))
               {
                  *((int32_t*)m_value) = iv;
                  change_made = true;
               }
               break;

            case UInt8:
               uv = strtoul(buf, NULL, 0);
//               if (dbg) Serial << __PRETTY_FUNCTION__ << "UInt8 = " << (int)uv;
//               if (dbg) Serial << strcpy_P(pbuf, PSTR("'\r\n"));
               if ( uv < 256 && uv > 0 &&
                     (m_umin == m_umax ||
                     (uv >= m_umin && uv <= m_umax ))) 
               {
                  *((uint8_t*)m_value) = uv;
                  change_made = true;
               }
               break;

            case UInt16:
               uv = strtoul(buf, NULL, 0);
//               if (dbg) Serial << __PRETTY_FUNCTION__ << "UInt16 = " << (int)uv;
//               if (dbg) Serial << strcpy_P(pbuf, PSTR("'\r\n"));
               if ( uv < 65536 && uv > 0 &&
                     (m_umin == m_umax ||
                     (uv >= m_umin && uv <= m_umax ))) 
               {
                  *((uint16_t*)m_value) = uv;
                  change_made = true;
               }
               break;

            case UInt32:
               uv = strtoul(buf, NULL, 0);
//               if (dbg) Serial << __PRETTY_FUNCTION__ << "UInt32 = " << (int)uv;
//               if (dbg) Serial << strcpy_P(pbuf, PSTR("'\r\n"));
               if ( m_umin == m_umax ||
                     (uv >= m_umin && uv <= m_umax ))
               {
                  *((uint32_t*)m_value) = uv;
                  change_made = true;
               }
               break;

            case String:
               char *cp;
               cp = (char*)m_value;
//               if (dbg) Serial << __PRETTY_FUNCTION__ << "char = " << cp;
//               if (dbg) Serial << strcpy_P(pbuf, PSTR("'\r\n"));
               while ( strlen((char*)m_value) < m_valueSize - 1 ) 
               {
                  char ch = *(buf++);
                  if ( ch >= '\x20' ) 
                  {
                     *(cp++) = ch;
                     *cp = '\0';
                  }
                  else 
                  {
                     break;
                  }
               }
               change_made = true;
               break;

            case TimeMin:
               int hour;
               int min;
               while (*buf && isspace(*buf))
                  buf++;
//               sscanf(buf, strcpy_P(pbuf, PSTR("%d:%d")), &hour, &min);
               sscanf(buf, "%d:%d", &hour, &min);
               Serial << strcpy_P(pbuf, PSTR("TimeMin Entry buf '")) << buf;
               Serial << strcpy_P(pbuf, PSTR("', hour = ")) << (int)hour;
               Serial << strcpy_P(pbuf, PSTR(", min = ")) << (int)min << endl;
               if (hour > 23 || hour < 0)
               {
                  Serial << strcpy_P(pbuf, PSTR("Invalid hour entered, should be 0 - 23\r\n"));
               }
               else if (min > 59 || min < 0)
               {
                  Serial << strcpy_P(pbuf, PSTR("Invalid minute entered, should be 0 - 59\r\n"));
               }
               else 
               {
                  *((uint16_t*)m_value) = (uint16_t)(hour * 60 + min);
                  change_made = true;
               }
//               if (dbg) Serial << __PRETTY_FUNCTION__ << "TimeInt = " << (uint16_t)*((uint16_t*)m_value);
//               if (dbg) Serial << strcpy_P(pbuf, PSTR("'\r\n"));
               break;

            case YesNo:
               while (*buf && isspace(*buf))
                  buf++;
               if ( tolower(*buf) == 'y' || tolower(*buf) == 't' ) 
               {
                  *((bool*)m_value) = true;
                  change_made = true;
               }
               else if ( tolower(*buf)== 'n' || tolower(*buf) == 'f' ) 
               {
                  *((bool*)m_value) = false;
                  change_made = true;
               }
               else 
               {
                  Serial << strcpy_P(pbuf, PSTR("Error- Unexpected value, expecting  'y' or 'n', 't' or 'f'"));
               }
               break;

            default:
               Serial << strcpy_P(pbuf, PSTR("Invalid nenu type encountered!!!"));
         }
         Serial << endl;
         PrintPath();
         Serial << "=";
         PrintValue();
         Serial << endl;
      }
   }
//   if (dbg) Serial << strcpy_P(pbuf, PSTR("Exit- Returning entry: ")) << strcpy_P(tbuf, rv->m_name) << endl;
   return(rv);
}

MenuEntry *MenuEntry::FindChild(const char *entry)
{
   MenuEntry *rv = NULL;
//   if (dbg) Serial << __PRETTY_FUNCTION__ << "entry = '" << entry << "'" << endl;
   if ( strlen(entry) == 1 ) 
   {
      char ch = *entry;
      if ( isalpha(ch) ) 
      {
         if ( ch <= 'Z' ) 
         {
            ch += '\x20';
         }
      }
      uint8_t idx = *(entry) - 'a';
      rv = m_1stChild;
      for (uint8_t x = 0; rv != NULL && x < idx; x++) 
      {
         rv = rv->m_nextSibling;
      }
   }
   else 
   {
      char name[256];
      char *cp;
      MenuEntry *it;
      for (it = m_1stChild; it != NULL; it = it->m_nextSibling)
      {
         strcpy_P(name, it->m_name);
         for (cp = name; *cp != '\0'; cp++) 
         {
            *cp = tolower(*cp);
         }
//         if (dbg) Serial << __PRETTY_FUNCTION__ << "name = '" << name << "'" << endl;
         if ( strcmp(name, entry) == 0 ) 
         {
            rv = it;
         }
      }
   }
   return(rv);
}

void MenuEntry::PrintChildren()
{
   for (MenuEntry *p = this; p != NULL; p = p->m_nextSibling ) 
   {
      if ( p->m_type == SubMenu || p->m_type == MainMenu )  
      {
         p->PrintEntry();
         p->m_1stChild->PrintChildren();
      }
   }
}
