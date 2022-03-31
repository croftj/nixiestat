#ifndef UISTATEMACHINE_H
#define UISTATEMACHINE_H

# include "UiState.h"
# include "ui.h"

class UI_Idle : public UIState
{
public:
   UI_Idle(const char *my_name);

   virtual void initEvent();
   virtual void upEvent();
   virtual void downEvent();
   virtual void modeEvent();
   virtual void stepEvent();
   virtual void setEvent();
   virtual void clearEvent();
   virtual void timeShortEvent();
   virtual void wakenEvent();
   virtual void onEntry();

protected:
private:
   uint8_t     m_index;
};

class UI_Awake : public UIState
{
public:
   UI_Awake(const char *my_name);
   virtual void upEvent();
   virtual void downEvent();
   virtual void modeEvent();
   virtual void stepEvent();
//   virtual void setEvent();
//   virtual void clearEvent();
   virtual void timeLongEvent();
   virtual void timeShortEvent();
//   virtual void wakenEvent();
   virtual void onEntry();

protected:
private:
};

class UI_ShowTemp : public UIState
{
public:
   UI_ShowTemp(const char *my_name);
   virtual void upEvent();
   virtual void downEvent();
   virtual void modeEvent();
   virtual void stepEvent();
   virtual void setEvent();
   virtual void clearEvent();
   virtual void timeLongEvent();
   virtual void timeShortEvent();
//   virtual void wakenEvent();
   virtual void onEntry();

protected:
private:
};

class UI_MinuteAdvance : public UIState
{
public:
   UI_MinuteAdvance(const char *my_name);
   virtual void upEvent();
   virtual void downEvent();
   virtual void modeEvent();
   virtual void stepEvent();
   virtual void setEvent();
   virtual void clearEvent();
   virtual void timeLongEvent();
//   virtual void timeShortEvent();
//   virtual void wakenEvent();
   virtual void onEntry();

protected:
private:
};

class UI_HourAdvance : public UIState
{
public:
   UI_HourAdvance(const char *my_name);
   virtual void upEvent();
   virtual void downEvent();
   virtual void modeEvent();
   virtual void stepEvent();
   virtual void setEvent();
   virtual void clearEvent();
   virtual void timeLongEvent();
//   virtual void timeShortEvent();
//   virtual void wakenEvent();
   virtual void onEntry();

protected:
private:
};

class UI_ModeAdvance : public UIState
{
public:
   UI_ModeAdvance(const char *my_name);
   virtual void upEvent();
   virtual void downEvent();
   virtual void modeEvent();
   virtual void stepEvent();
   virtual void setEvent();
   virtual void clearEvent();
   virtual void timeLongEvent();
//   virtual void timeShortEvent();
//   virtual void wakenEvent();
   virtual void onEntry();

protected:
private:
};

class UI_TemperatureAdvance : public UIState
{
public:
   UI_TemperatureAdvance(const char *my_name);
   virtual void upEvent();
   virtual void downEvent();
   virtual void modeEvent();
   virtual void stepEvent();
   virtual void setEvent();
   virtual void clearEvent();
   virtual void timeLongEvent();
//   virtual void timeShortEvent();
//   virtual void wakenEvent();
   virtual void onEntry();

protected:
private:
};

class UI_DayAdvance : public UIState
{
public:
   UI_DayAdvance(const char *my_name);
   virtual void upEvent();
   virtual void downEvent();
   virtual void modeEvent();
   virtual void stepEvent();
   virtual void setEvent();
   virtual void clearEvent();
   virtual void timeLongEvent();
//   virtual void timeShortEvent();
//   virtual void wakenEvent();
   virtual void onEntry();

protected:
private:
};

class UI_MonthAdvance : public UIState
{
public:
   UI_MonthAdvance(const char *my_name);
   virtual void upEvent();
   virtual void downEvent();
   virtual void modeEvent();
   virtual void stepEvent();
   virtual void setEvent();
   virtual void clearEvent();
   virtual void timeLongEvent();
//   virtual void timeShortEvent();
//   virtual void wakenEvent();
   virtual void onEntry();

protected:
private:
};

class UI_YearAdvance : public UIState
{
public:
   UI_YearAdvance(const char *my_name);
   virtual void upEvent();
   virtual void downEvent();
   virtual void modeEvent();
   virtual void stepEvent();
   virtual void setEvent();
   virtual void clearEvent();
   virtual void timeLongEvent();
//   virtual void timeShortEvent();
//   virtual void wakenEvent();
   virtual void onEntry();

protected:
private:
};

class UI_HeatCool : public UIState
{
public:
   UI_HeatCool(const char *my_name);
   virtual void upEvent();
   virtual void downEvent();
   virtual void modeEvent();
   virtual void stepEvent();
   virtual void setEvent();
   virtual void clearEvent();
   virtual void timeLongEvent();
//   virtual void timeShortEvent();
//   virtual void wakenEvent();
   virtual void onEntry();

protected:
private:
};


#endif
