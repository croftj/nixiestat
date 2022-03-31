# include "UiStateMachine.h"

#include "esp_log.h"

#include "configuration.h"
# include "ui.h"

#define KEY __PRETTY_FUNCTION__

extern Configuration *config;

static UI_Idle                state_Idle("Idle");
static UI_Awake               state_Awake("Awake");
static UI_ShowTemp            state_ShowTemp("ShowTemp");
static UI_MinuteAdvance       state_MinuteAdvance("MinuteAdvance");
static UI_HourAdvance         state_HourAdvance("HourAdvance");
static UI_TemperatureAdvance  state_TemperatureAdvance("TemperatureAdvance");
static UI_DayAdvance          state_DayAdvance("DayAdvance");
static UI_MonthAdvance        state_MonthAdvance("MonthAdvance");
static UI_YearAdvance         state_YearAdvance("YearAdvance");
static UI_HeatCool            state_HeatCool("HeatCool");
static UI_ModeAdvance         state_ModeAdvance("ModeAdvance");

UIState*  UIState::m_initialState;

/**********************************************************************/
/*   Idle  State,  this state disables the display and waits for any  */
/*   activity on the touch pannel                                     */
/**********************************************************************/
UI_Idle::UI_Idle(const char *my_name) : UIState(my_name), m_index(1)
{
   m_initialState = this;
   m_index = 0;
}

void UI_Idle::initEvent()
{
   ui.setState( &state_Awake );
}

void UI_Idle::upEvent()
{
   ui.setState( &state_ShowTemp );
}

void UI_Idle::downEvent()
{
   ui.setState( &state_ShowTemp );
}

void UI_Idle::modeEvent()
{
   ui.setState( &state_Awake );
}

void UI_Idle::stepEvent()
{
   ui.setState( &state_ShowTemp );
}

void UI_Idle::setEvent()
{
   ui.setState( &state_ShowTemp );
}

void UI_Idle::clearEvent()
{
   ui.setState( &state_ShowTemp );
}

void UI_Idle::timeShortEvent()
{
   m_index++;
   if ( m_index > 3 ) 
   {
      m_index = 0;
   }
}

void UI_Idle::wakenEvent()
{
   ui.setState( &state_Awake );
}


void UI_Idle::onEntry()
{
   ui.clearLamps();
   ui.flashNone();
   ui.displayOff();
}


/*************************************************************/
/* Awake  State,  this  state  lets you raise or lower       */
/* the temp.                                                 */
/*                                                           */
/* This state initialy shows the current time then after the */
/* short time event moves to the showTemp state if another   */
/* action does not happen before that.                       */
/*                                                           */
/* This  and the ShowTemp state are tied at the hips in      */
/* a way. If you change an action  for an event here, you    */
/* should check if it needs changed in the other.            */
/*************************************************************/
UI_Awake::UI_Awake(const char *my_name) : UIState(my_name)
{
}

void UI_Awake::upEvent()
{
   ui.setState( &state_ShowTemp );
}

void UI_Awake::downEvent()
{
   ui.setState( &state_ShowTemp );
}

void UI_Awake::modeEvent()
{
   ui.setSetting(Ui::TIME_SETTING_1);
   ui.setState( &state_ModeAdvance );
}

void UI_Awake::stepEvent()
{
   ui.setState( &state_HeatCool );
}

void UI_Awake::timeLongEvent()
{
   ui.setState( &state_Idle );
}

void UI_Awake::timeShortEvent()
{
   ESP_LOGI(KEY, "short event");
   ui.setState( &state_ShowTemp );
}

void UI_Awake::onEntry()
{
   ui.clearLamps();
   int tod = config->adjustedTimeOfDay() / 60;
   ESP_LOGI(KEY, "tod = %d, mins = %d, hours = %d", tod, tod / 60, tod % 60);

   ui.showMode();
   ui.showHours(tod / 60);
   ui.showMinutes(tod % 60);
   ui.displayOn();
}

/**********************************************************************/
/*   ShowTemp  State,  this  state  lets you raise or lower the temp. */
/*                                                                    */
/*   This  and the Awake state are tied at the hips in a way. If you  */
/*   change  an  action  for  an  event here, you should check if it  */
/*   needs changed in the other                                       */
/**********************************************************************/
UI_ShowTemp::UI_ShowTemp(const char *my_name) : UIState(my_name)
{
}

void UI_ShowTemp::upEvent()
{
//   ui.setState( &state_Awake );
//   ui.flashTemperature();
   ui.setSetting(Ui::CURRENT_SETTINGS);
   ESP_LOGI(KEY, "increment");
   ui.incrTemperature();   
//   ui.saveTemperature();
}

void UI_ShowTemp::downEvent()
{
//   ui.setState( &state_Awake );
//   ui.flashTemperature();
   ui.setSetting(Ui::CURRENT_SETTINGS);
   ESP_LOGI(KEY, "decrement");
   ui.decrTemperature();   
//   ui.saveTemperature();
}

void UI_ShowTemp::setEvent()
{
   ui.flashNone();
   ui.setState( &state_Awake );
}

void UI_ShowTemp::clearEvent()
{
   ui.flashNone();
   ui.setState( &state_Awake );
}

void UI_ShowTemp::modeEvent()
{
   ui.setState( &state_ModeAdvance );
}

void UI_ShowTemp::stepEvent()
{
   ui.setState( &state_HeatCool );
}

void UI_ShowTemp::timeLongEvent()
{
   ui.setState( &state_Idle );
}

void UI_ShowTemp::timeShortEvent()
{
   uint8_t setting = ui.getSetting();
   if (setting < Ui::NUM_SETTINGS)
   {
      ui.showTemperature(setting, (bool)Ui::TEMP_BOTTOM);
      ui.blankTemperature(Ui::TEMP_BOTTOM, false);
      ui.showSettings();
      ui.setSetting(++setting);
   }
   else
   {
      ui.setState( &state_Idle );
   }
}

void UI_ShowTemp::onEntry()
{
   ui.showMode();
   ui.setSetting(Ui::TIME_SETTING_1);
   ui.showTemperature((uint8_t)Ui::CURRENT_SETTINGS, (bool)Ui::TEMP_TOP);
   ui.showOffset();
//   ui.showTempOffset();
   ui.blankTemperature(Ui::TEMP_BOTTOM, true);
   ui.displayOn();
}

/**********************************************************************/
/*   state_HeatCool, toggles heating or cooling                       */
/**********************************************************************/
UI_HeatCool::UI_HeatCool(const char *my_name) : UIState(my_name)
{
}

void UI_HeatCool::upEvent()
{
   ui.toggleHeatCool();
}

void UI_HeatCool::downEvent()
{
   ui.toggleHeatCool();
}

void UI_HeatCool::modeEvent()
{
   ui.setState( &state_ModeAdvance );
}

void UI_HeatCool::stepEvent()
{
   ui.setState( &state_HourAdvance );
}

void UI_HeatCool::setEvent()
{
   ui.flashNone();
   ui.saveSettings();
   ui.setState( &state_Awake );
}

void UI_HeatCool::clearEvent()
{
   ui.flashNone();
   ui.restoreSettings();
   ui.setState( &state_Awake );
}

void UI_HeatCool::timeLongEvent()
{
   ui.setState( &state_Awake );
}

void UI_HeatCool::onEntry()
{
   ui.flashHeatCool();
}

/**********************************************************************/
/*   state_ModeAdvance,  Here  we  incrememnt the temperature  */
/*   of the current setting                                           */
/**********************************************************************/
UI_ModeAdvance::UI_ModeAdvance(const char *my_name) : UIState(my_name)
{
}

void UI_ModeAdvance::upEvent()
{
   ui.setState( &state_TemperatureAdvance );
}

void UI_ModeAdvance::downEvent()
{
   ui.setState( &state_TemperatureAdvance );
}

void UI_ModeAdvance::modeEvent()
{
   ui.moveToNextSetting();
   ui.showSettings();
}

void UI_ModeAdvance::stepEvent()
{
   ui.setState( &state_TemperatureAdvance );
}

void UI_ModeAdvance::setEvent()
{
   ui.setState( &state_Awake );
}

void UI_ModeAdvance::clearEvent()
{
   ui.flashNone();
   ui.setState( &state_Awake );
}

void UI_ModeAdvance::timeLongEvent()
{
   ui.setState( &state_Idle );
}

void UI_ModeAdvance::onEntry()
{
   ui.restoreSettings();
   ui.flashNone();
}

/**********************************************************************/
/*   state_TemperatureAdvance,  Here  we  incrememnt the temperature  */
/*   of the current setting                                           */
/**********************************************************************/
UI_TemperatureAdvance::UI_TemperatureAdvance(const char *my_name) : UIState(my_name)
{
}

void UI_TemperatureAdvance::upEvent()
{
   ui.incrTemperature();
   if ( ui.setting() == Ui::CURRENT_SETTINGS ) 
   {
      ui.saveTemperature();
   }
}

void UI_TemperatureAdvance::downEvent()
{
   ui.decrTemperature();
   if ( ui.setting() == Ui::CURRENT_SETTINGS ) 
   {
      ui.saveTemperature();
   }
}

void UI_TemperatureAdvance::modeEvent()
{
   ui.restoreSettings();
   ui.moveToNextSetting();
   ui.showSettings();
   ui.setState( &state_ModeAdvance );
}

void UI_TemperatureAdvance::stepEvent()
{
   ui.setState( &state_HourAdvance );
}

void UI_TemperatureAdvance::setEvent()
{
   ui.saveSettings();
   ui.setState( &state_Awake );
}

void UI_TemperatureAdvance::clearEvent()
{
   ui.flashNone();
   ui.restoreSettings();
   ui.setState( &state_Awake );
}

void UI_TemperatureAdvance::timeLongEvent()
{
   ui.setState( &state_Idle );
}

void UI_TemperatureAdvance::onEntry()
{
   ui.restoreSettings();
   ui.flashTemperature();
}


/**********************************************************************/
/*   state_HourAdvance,  Here we allow the Primary Mode Minutes  */
/*   be changed                                                       */
/**********************************************************************/
UI_HourAdvance::UI_HourAdvance(const char *my_name) : UIState(my_name)
{
}

void UI_HourAdvance::upEvent()
{
   ui.incrHours();
}

void UI_HourAdvance::downEvent()
{
   ui.decrHours();
}

void UI_HourAdvance::modeEvent()
{
   if ( ! m_changesActive ) 
   {
      ui.moveToNextSetting();
      ui.setState( &state_TemperatureAdvance );
      m_changesActive = true;
   }
}

void UI_HourAdvance::stepEvent()
{
   ui.setState( &state_MinuteAdvance );
}

void UI_HourAdvance::setEvent()
{
   ui.saveSettings(true);
   ui.setState( &state_Awake );
   m_changesActive = false;
   ui.setState( &state_MinuteAdvance );
}

void UI_HourAdvance::clearEvent()
{
   ui.setState( &state_Awake );
   ui.flashNone();
   ui.restoreSettings();
   m_changesActive = false;
}

void UI_HourAdvance::timeLongEvent()
{
   ui.setState( &state_Awake );
}

void UI_HourAdvance::onEntry()
{
   ui.flashHours();
   m_changesActive = false;
}

/**********************************************************************/
/*   state_MinuteAdvance,  Here we allow the Primary Mode Minutes  */
/*   be changed                                                       */
/**********************************************************************/
UI_MinuteAdvance::UI_MinuteAdvance(const char *my_name) : UIState(my_name)
{
}

void UI_MinuteAdvance::upEvent()
{
   ui.incrMinutes();
   m_changesActive = true;
}

void UI_MinuteAdvance::downEvent()
{
   ui.decrMinutes();
   m_changesActive = true;
}

void UI_MinuteAdvance::modeEvent()
{
   if ( ! m_changesActive ) 
   {
      ui.moveToNextSetting();
      ui.setState( &state_TemperatureAdvance );
   }
}

void UI_MinuteAdvance::stepEvent()
{
   if ( ! m_changesActive ) 
   {
      if ( ui.haveCalendar() ) 
      {
         ui.setState( &state_MonthAdvance );
      }
      else 
      {
         ui.setState( &state_TemperatureAdvance );
      }
   }
}

void UI_MinuteAdvance::setEvent()
{
   ui.saveSettings(true);
   ui.flashMinutes();
   m_changesActive = false;
}

void UI_MinuteAdvance::clearEvent()
{
   ui.setState( &state_Awake );
   ui.flashNone();
   ui.restoreSettings();
   m_changesActive = false;
}

void UI_MinuteAdvance::timeLongEvent()
{
   ui.setState( &state_Idle );
   m_changesActive = false;
}

void UI_MinuteAdvance::onEntry()
{
   ui.flashMinutes();
   ui.showSettings();
}

/**********************************************************************/
/*   state_MonthAdvance, Increments and decrememnts the month         */
/**********************************************************************/
UI_MonthAdvance::UI_MonthAdvance(const char *my_name) : UIState(my_name)
{
}

void UI_MonthAdvance::upEvent()
{
   ui.incrMonth();
}

void UI_MonthAdvance::downEvent()
{
   ui.decrMonth();
}

void UI_MonthAdvance::modeEvent()
{
   ui.moveToNextSetting();
   ui.setState( &state_TemperatureAdvance );
}

void UI_MonthAdvance::stepEvent()
{
   ui.setState( &state_DayAdvance );
}

void UI_MonthAdvance::setEvent()
{
   ui.saveSettings(true);
   ui.setState( &state_Awake );
}

void UI_MonthAdvance::clearEvent()
{
   ui.flashNone();
   ui.setState( &state_Awake );
}

void UI_MonthAdvance::timeLongEvent()
{
   ui.setState( &state_Awake );
}

void UI_MonthAdvance::onEntry()
{
   ui.flashMonth();
}

/**********************************************************************/
/*   state_DayAdvance, Incrememnets and decrememnet the day           */
/**********************************************************************/
UI_DayAdvance::UI_DayAdvance(const char *my_name) : UIState(my_name)
{
}

void UI_DayAdvance::upEvent()
{
   ui.incrDay();
}

void UI_DayAdvance::downEvent()
{
   ui.decrDay();
}

void UI_DayAdvance::modeEvent()
{
   ui.moveToNextSetting();
   ui.setState( &state_TemperatureAdvance );
}

void UI_DayAdvance::stepEvent()
{
   ui.setState( &state_YearAdvance );
}

void UI_DayAdvance::setEvent()
{
   ui.saveSettings(true);
   ui.setState( &state_Awake );
}

void UI_DayAdvance::clearEvent()
{
   ui.flashNone();
   ui.setState( &state_Awake );
}

void UI_DayAdvance::timeLongEvent()
{
   ui.setState( &state_Awake );
}

void UI_DayAdvance::onEntry()
{
   ui.flashDay();
}

/**********************************************************************/
/*   state_YearAdvance, intecrememnts the year                        */
/**********************************************************************/
UI_YearAdvance::UI_YearAdvance(const char *my_name) : UIState(my_name)
{
}

void UI_YearAdvance::upEvent()
{
   ui.incrYear();
}

void UI_YearAdvance::downEvent()
{
   ui.decrYear();
}

void UI_YearAdvance::modeEvent()
{
   ui.moveToNextSetting();
   ui.setState( &state_TemperatureAdvance );
}

void UI_YearAdvance::stepEvent()
{
   ui.setState( &state_TemperatureAdvance );
}

void UI_YearAdvance::setEvent()
{
   ui.saveSettings(true);
   ui.setState( &state_Awake );
}

void UI_YearAdvance::clearEvent()
{
   ui.flashNone();
   ui.setState( &state_Awake );
}

void UI_YearAdvance::timeLongEvent()
{
   ui.setState( &state_Awake );
}

void UI_YearAdvance::onEntry()
{
   ui.flashYear();
}
