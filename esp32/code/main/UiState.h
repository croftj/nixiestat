#ifndef UISTATE_H
#define UISTATE_H

# include <string.h>

class UIState 
{
public:
   UIState(const char *my_name)
   {
      memset(m_name, '\0', sizeof(m_name));
      strncpy(m_name, my_name, sizeof(m_name) - 1);
   }

   const char *name()
   {
      return(m_name);
   }

   virtual void initEvent()      {};
   virtual void anyKeyEvent()    {};
   virtual void upEvent()        {};
   virtual void downEvent()      {};
   virtual void modeEvent()      {};
   virtual void stepEvent()      {};
   virtual void setEvent()       {};
   virtual void clearEvent()     {};
   virtual void timeLongEvent()  {};
   virtual void timeShortEvent() {};
   virtual void onEntry()        {};

   static UIState *initialState()
   {
      return(m_initialState);
   }

protected:
   static UIState *m_initialState;
   char           m_name[32];
   bool           m_changesActive;

private:
};

#endif
