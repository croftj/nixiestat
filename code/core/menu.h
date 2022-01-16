#ifndef MENU_H
#define MENU_H

#include <stddef.h>
#include <stdint.h>

class MenuEntry 
{
public:
   typedef enum
   {
      Action,
      CustomIO,
      Float,
      Int8,
      Int16,
      Int32,
      String,
      TimeMin,
      UInt8,
      UInt16,
      UInt32,
      YesNo,
      MainMenu,
      SubMenu
   } MenuType_t;

   MenuEntry(const char *name, MenuType_t type, float* value, MenuEntry *parent, float min=0.0, float max=0.0);
   MenuEntry(const char *name, MenuType_t type, int* value, MenuEntry *parent, int min=0, int max=0);
   MenuEntry(const char *name, MenuType_t type, unsigned int* value, MenuEntry *parent, unsigned int min=0, unsigned int max=0);
   MenuEntry(const char *name, MenuType_t type, void* value, uint8_t val_size, MenuEntry *parent);
   MenuEntry(const char *name, MenuType_t type, bool (*input)(const char*), void (*output)(), MenuEntry *parent);
   MenuEntry(const char *name, MenuType_t type, bool (*action)(), MenuEntry *parent);
   MenuEntry(const char *name, MenuType_t type, MenuEntry *parent = NULL);

   void PrintChildren();
   void PrintPath();
   bool PrintEntry();
   bool PrintValue();
   MenuEntry *Execute(char *buf, MenuEntry *first_menu, bool &change_made);
   MenuEntry *Parent()
   {
      if ( m_parent == NULL ) 
      {
         return(this);
      }
      else 
      {
         return(m_parent);
      }
   }

   char *Name()
   {
      return((char*)m_name);
   }
private:
   void           AddChildMenu(MenuEntry *child);
   MenuEntry*     FindChild(const char *entry);

   const char     *m_name;
   MenuType_t     m_type;
   uint8_t        m_valueSize;
   void           *m_value;
   bool           (*m_action)();
   bool           (*m_input)(const char*);
   void           (*m_output)();
   double         m_fmin;
   double         m_fmax;
   int            m_imin;
   int            m_imax;
   unsigned int   m_umin;
   unsigned int   m_umax;
   MenuEntry      *m_parent;
   MenuEntry      *m_1stChild;
   MenuEntry      *m_nextSibling;
};

#endif
