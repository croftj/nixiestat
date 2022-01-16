# include "UiStateMachine.h"

# include "datetime.h"
# include "ui.h"

static uint8_t                dbg = true;
static uint8_t                dbg1 = true;

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
   if (dbg) Serial << __PRETTY_FUNCTION__ << " here" << endl;
//   ui.setState( &state_Awake );
}

void UI_Idle::upEvent()
{
   if (dbg) Serial << __PRETTY_FUNCTION__ << " here" << endl;
//   ui.setState( &state_Awake );
}

void UI_Idle::downEvent()
{
   if (dbg) Serial << __PRETTY_FUNCTION__ << " here" << endl;
//   ui.setState( &state_Awake );
}

void UI_Idle::modeEvent()
{
   if (dbg) Serial << __PRETTY_FUNCTION__ << " here" << endl;
//   ui.setState( &state_Awake );
}

void UI_Idle::stepEvent()
{
   if (dbg) Serial << __PRETTY_FUNCTION__ << " here" << endl;
//   ui.setState( &state_Awake );
}

void UI_Idle::setEvent()
{
   if (dbg) Serial << __PRETTY_FUNCTION__ << " here" << endl;
   ui.saveSettings();
//   ui.setState( &state_Awake );
}

void UI_Idle::clearEvent()
{
   if (dbg) Serial << __PRETTY_FUNCTION__ << " here" << endl;
   ui.flashNone();
//   ui.setState( &state_Awake );
}

void UI_Idle::timeShortEvent()
{
   m_index++;
   if ( m_index > 3 ) 
   {
      m_index = 0;
   }
   ui.ShowTimeTemp( m_index );
}

void UI_Idle::wakenEvent()
{
   if (dbg) Serial << __PRETTY_FUNCTION__ << " here" << endl;
//   ui.setState( &state_Awake );
}


void UI_Idle::onEntry()
{
   if (dbg) Serial << __PRETTY_FUNCTION__ << " here" << endl;
   ui.clearLamps();
   ui.flashNone();
//   ui.displayOff();
}


/**********************************************************************/
/*   Awake  State,  this  state  lets you raise or lower the temp.    */
/*                                                                    */
/*   This  and the ShowTemp state are tied at the hips in a way. If   */
/*   you change an action  for an event here, you should check if it  */
/*   needs changed in the other                                       */
/**********************************************************************/
UI_Awake::UI_Awake(const char *my_name) : UIState(my_name)
{
}

void UI_Awake::upEvent()
{
   if (dbg1) Serial << __PRETTY_FUNCTION__ << " here" << endl;
   ui.setState( &state_ShowTemp );
}

void UI_Awake::downEvent()
{
   if (dbg1) Serial << __PRETTY_FUNCTION__ << " here" << endl;
   ui.setState( &state_ShowTemp );
}

void UI_Awake::modeEvent()
{
   if (dbg1) Serial << __PRETTY_FUNCTION__ << " here" << endl;
   ui.setSetting(Ui::TIME_SETTING_1);
   ui.setState( &state_ModeAdvance );
}

void UI_Awake::stepEvent()
{
   if (dbg) Serial << __PRETTY_FUNCTION__ << " here" << endl;
   ui.setState( &state_HeatCool );
}

void UI_Awake::timeLongEvent()
{
   if (dbg) Serial << __PRETTY_FUNCTION__ << " here" << endl;
   ui.setState( &state_Idle );
}

void UI_Awake::timeShortEvent()
{
   if (dbg) Serial << __PRETTY_FUNCTION__ << " here" << endl;
   ui.setState( &state_ShowTemp );
}

void UI_Awake::onEntry()
{
   uint16_t dt[7];

   if (dbg) Serial << __PRETTY_FUNCTION__ << " here" << endl;
   ui.clearLamps();
   ui.showCurrentTemp();
   currentDate.getTime(dt);
   display.setDigit(0, dt[Clock::Hour] / 10);
   display.setDigit(1, dt[Clock::Hour] % 10);
   display.lamp(Display::LAMP_LOWER_COLON, true);
   display.lamp(Display::LAMP_UPPER_COLON, true);
   display.setDigit(2, dt[Clock::Minute] / 10);
   display.setDigit(3, dt[Clock::Minute] % 10);
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
   if (dbg) Serial << __PRETTY_FUNCTION__ << " here" << endl;
//   ui.setState( &state_Awake );
   ui.flashTemperature();
   ui.incrTemperature();   
   ui.saveTemperature();
}

void UI_ShowTemp::downEvent()
{
   if (dbg) Serial << __PRETTY_FUNCTION__ << " here" << endl;
//   ui.setState( &state_Awake );
   ui.flashTemperature();
   ui.decrTemperature();   
   ui.saveTemperature();
}

void UI_ShowTemp::setEvent()
{
   if (dbg) Serial << __PRETTY_FUNCTION__ << " here" << endl;
   ui.flashNone();
   ui.setState( &state_Awake );
}

void UI_ShowTemp::clearEvent()
{
   if (dbg) Serial << __PRETTY_FUNCTION__ << " here" << endl;
   ui.flashNone();
   ui.setState( &state_Awake );
}

void UI_ShowTemp::modeEvent()
{
   if (dbg) Serial << __PRETTY_FUNCTION__ << " here" << endl;
   ui.setState( &state_ModeAdvance );
}

void UI_ShowTemp::stepEvent()
{
   if (dbg) Serial << __PRETTY_FUNCTION__ << " here" << endl;
   ui.setState( &state_HeatCool );
}

void UI_ShowTemp::timeLongEvent()
{
   if (dbg) Serial << __PRETTY_FUNCTION__ << " here" << endl;
   ui.setState( &state_Idle );
}

void UI_ShowTemp::timeShortEvent()
{
   if (dbg) Serial << __PRETTY_FUNCTION__ << " here" << endl;
   uint8_t setting = ui.getSetting();
   if (dbg) Serial << __PRETTY_FUNCTION__ << " setting = " << (int)setting << endl;
   ui.setSetting(setting);
}

void UI_ShowTemp::onEntry()
{
   if (dbg) Serial << __PRETTY_FUNCTION__ << " here" << endl;
   ui.setSetting(Ui::CURRENT_SETTINGS);
   ui.flashNone();
   ui.restoreSettings();
   ui.showSettings();
}

/**********************************************************************/
/*   state_HeatCool, toggles heating or cooling                       */
/**********************************************************************/
UI_HeatCool::UI_HeatCool(const char *my_name) : UIState(my_name)
{
}

void UI_HeatCool::upEvent()
{
   if (dbg) Serial << __PRETTY_FUNCTION__ << " here" << endl;
   ui.toggleHeatCool();
}

void UI_HeatCool::downEvent()
{
   if (dbg) Serial << __PRETTY_FUNCTION__ << " here" << endl;
   ui.toggleHeatCool();
}

void UI_HeatCool::modeEvent()
{
   if (dbg) Serial << __PRETTY_FUNCTION__ << " here" << endl;
   ui.setState( &state_ModeAdvance );
}

void UI_HeatCool::stepEvent()
{
   if (dbg) Serial << __PRETTY_FUNCTION__ << " here" << endl;
   ui.setState( &state_HourAdvance );
}

void UI_HeatCool::setEvent()
{
   if (dbg) Serial << __PRETTY_FUNCTION__ << " here" << endl;
   ui.flashNone();
   ui.saveSettings();
   ui.setState( &state_Awake );
}

void UI_HeatCool::clearEvent()
{
   if (dbg) Serial << __PRETTY_FUNCTION__ << " here" << endl;
   ui.flashNone();
   ui.restoreSettings();
   ui.setState( &state_Awake );
}

void UI_HeatCool::timeLongEvent()
{
   if (dbg) Serial << __PRETTY_FUNCTION__ << " here" << endl;
   ui.setState( &state_Awake );
}

void UI_HeatCool::onEntry()
{
   if (dbg) Serial << __PRETTY_FUNCTION__ << " here" << endl;
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
   if (dbg) Serial << __PRETTY_FUNCTION__ << " here" << endl;
   ui.setState( &state_TemperatureAdvance );
}

void UI_ModeAdvance::downEvent()
{
   if (dbg) Serial << __PRETTY_FUNCTION__ << " here" << endl;
   ui.setState( &state_TemperatureAdvance );
}

void UI_ModeAdvance::modeEvent()
{
   if (dbg) Serial << __PRETTY_FUNCTION__ << " here" << endl;
   ui.moveToNextSetting();
   ui.showSettings();
}

void UI_ModeAdvance::stepEvent()
{
   if (dbg) Serial << __PRETTY_FUNCTION__ << " here" << endl;
   ui.setState( &state_TemperatureAdvance );
}

void UI_ModeAdvance::setEvent()
{
   if (dbg) Serial << __PRETTY_FUNCTION__ << " here" << endl;
   ui.setState( &state_Awake );
}

void UI_ModeAdvance::clearEvent()
{
   if (dbg) Serial << __PRETTY_FUNCTION__ << " here" << endl;
   ui.flashNone();
   ui.setState( &state_Awake );
}

void UI_ModeAdvance::timeLongEvent()
{
   if (dbg) Serial << __PRETTY_FUNCTION__ << " here" << endl;
   ui.setState( &state_Idle );
}

void UI_ModeAdvance::onEntry()
{
   if (dbg) Serial << __PRETTY_FUNCTION__ << " here" << endl;
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
   if (dbg) Serial << __PRETTY_FUNCTION__ << " here" << endl;
   ui.incrTemperature();
   if ( ui.setting() == Ui::CURRENT_SETTINGS ) 
   {
      ui.saveTemperature();
   }
}

void UI_TemperatureAdvance::downEvent()
{
   if (dbg) Serial << __PRETTY_FUNCTION__ << " here" << endl;
   ui.decrTemperature();
   if ( ui.setting() == Ui::CURRENT_SETTINGS ) 
   {
      ui.saveTemperature();
   }
}

void UI_TemperatureAdvance::modeEvent()
{
   if (dbg) Serial << __PRETTY_FUNCTION__ << " here" << endl;
   ui.restoreSettings();
   ui.moveToNextSetting();
   ui.showSettings();
   ui.setState( &state_ModeAdvance );
}

void UI_TemperatureAdvance::stepEvent()
{
   if (dbg) Serial << __PRETTY_FUNCTION__ << " here" << endl;
   ui.setState( &state_HourAdvance );
}

void UI_TemperatureAdvance::setEvent()
{
   if (dbg) Serial << __PRETTY_FUNCTION__ << " here" << endl;
   ui.saveSettings();
   ui.setState( &state_Awake );
}

void UI_TemperatureAdvance::clearEvent()
{
   if (dbg) Serial << __PRETTY_FUNCTION__ << " here" << endl;
   ui.flashNone();
   ui.restoreSettings();
   ui.setState( &state_Awake );
}

void UI_TemperatureAdvance::timeLongEvent()
{
   if (dbg) Serial << __PRETTY_FUNCTION__ << " here" << endl;
   ui.setState( &state_Idle );
}

void UI_TemperatureAdvance::onEntry()
{
   if (dbg) Serial << __PRETTY_FUNCTION__ << " here" << endl;
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
   if (dbg) Serial << __PRETTY_FUNCTION__ << " here" << endl;
   ui.incrHours();
}

void UI_HourAdvance::downEvent()
{
   if (dbg) Serial << __PRETTY_FUNCTION__ << " here" << endl;
   ui.decrHours();
}

void UI_HourAdvance::modeEvent()
{
   if (dbg) Serial << __PRETTY_FUNCTION__ << " here" << endl;
   if ( ! m_changesActive ) 
   {
      ui.moveToNextSetting();
      ui.setState( &state_TemperatureAdvance );
      m_changesActive = true;
   }
}

void UI_HourAdvance::stepEvent()
{
   if (dbg) Serial << __PRETTY_FUNCTION__ << " here" << endl;
   ui.setState( &state_MinuteAdvance );
}

void UI_HourAdvance::setEvent()
{
   if (dbg) Serial << __PRETTY_FUNCTION__ << " here" << endl;
   ui.saveSettings(true);
   ui.setState( &state_Awake );
   m_changesActive = false;
   ui.setState( &state_MinuteAdvance );
}

void UI_HourAdvance::clearEvent()
{
   if (dbg) Serial << __PRETTY_FUNCTION__ << " here" << endl;
   ui.setState( &state_Awake );
   ui.flashNone();
   ui.restoreSettings();
   m_changesActive = false;
}

void UI_HourAdvance::timeLongEvent()
{
   if (dbg) Serial << __PRETTY_FUNCTION__ << " here" << endl;
   ui.setState( &state_Awake );
}

void UI_HourAdvance::onEntry()
{
   if (dbg) Serial << __PRETTY_FUNCTION__ << " here" << endl;
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
   if (dbg) Serial << __PRETTY_FUNCTION__ << " here" << endl;
   ui.incrMinutes();
   m_changesActive = true;
}

void UI_MinuteAdvance::downEvent()
{
   if (dbg) Serial << __PRETTY_FUNCTION__ << " here" << endl;
   ui.decrMinutes();
   m_changesActive = true;
}

void UI_MinuteAdvance::modeEvent()
{
   if (dbg) Serial << __PRETTY_FUNCTION__ << " here" << endl;
   if ( ! m_changesActive ) 
   {
      ui.moveToNextSetting();
      ui.setState( &state_TemperatureAdvance );
   }
}

void UI_MinuteAdvance::stepEvent()
{
   if (dbg) Serial << __PRETTY_FUNCTION__ << " here" << endl;
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
   if (dbg) Serial << __PRETTY_FUNCTION__ << " here" << endl;
   ui.saveSettings(true);
   ui.flashMinutes();
   m_changesActive = false;
}

void UI_MinuteAdvance::clearEvent()
{
   if (dbg) Serial << __PRETTY_FUNCTION__ << " here" << endl;
   ui.setState( &state_Awake );
   ui.flashNone();
   ui.restoreSettings();
   m_changesActive = false;
}

void UI_MinuteAdvance::timeLongEvent()
{
   if (dbg) Serial << __PRETTY_FUNCTION__ << " here" << endl;
   ui.setState( &state_Idle );
   m_changesActive = false;
}

void UI_MinuteAdvance::onEntry()
{
   if (dbg) Serial << __PRETTY_FUNCTION__ << " here" << endl;
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
   if (dbg) Serial << __PRETTY_FUNCTION__ << " here" << endl;
   ui.incrMonth();
}

void UI_MonthAdvance::downEvent()
{
   if (dbg) Serial << __PRETTY_FUNCTION__ << " here" << endl;
   ui.decrMonth();
}

void UI_MonthAdvance::modeEvent()
{
   if (dbg) Serial << __PRETTY_FUNCTION__ << " here" << endl;
   ui.moveToNextSetting();
   ui.setState( &state_TemperatureAdvance );
}

void UI_MonthAdvance::stepEvent()
{
   if (dbg) Serial << __PRETTY_FUNCTION__ << " here" << endl;
   ui.setState( &state_DayAdvance );
}

void UI_MonthAdvance::setEvent()
{
   if (dbg) Serial << __PRETTY_FUNCTION__ << " here" << endl;
   ui.saveSettings(true);
   ui.setState( &state_Awake );
}

void UI_MonthAdvance::clearEvent()
{
   if (dbg) Serial << __PRETTY_FUNCTION__ << " here" << endl;
   ui.flashNone();
   ui.setState( &state_Awake );
}

void UI_MonthAdvance::timeLongEvent()
{
   if (dbg) Serial << __PRETTY_FUNCTION__ << " here" << endl;
   ui.setState( &state_Awake );
}

void UI_MonthAdvance::onEntry()
{
   if (dbg) Serial << __PRETTY_FUNCTION__ << " here" << endl;
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
   if (dbg) Serial << __PRETTY_FUNCTION__ << " here" << endl;
   ui.incrDay();
}

void UI_DayAdvance::downEvent()
{
   if (dbg) Serial << __PRETTY_FUNCTION__ << " here" << endl;
   ui.decrDay();
}

void UI_DayAdvance::modeEvent()
{
   if (dbg) Serial << __PRETTY_FUNCTION__ << " here" << endl;
   ui.moveToNextSetting();
   ui.setState( &state_TemperatureAdvance );
}

void UI_DayAdvance::stepEvent()
{
   if (dbg) Serial << __PRETTY_FUNCTION__ << " here" << endl;
   ui.setState( &state_YearAdvance );
}

void UI_DayAdvance::setEvent()
{
   if (dbg) Serial << __PRETTY_FUNCTION__ << " here" << endl;
   ui.saveSettings(true);
   ui.setState( &state_Awake );
}

void UI_DayAdvance::clearEvent()
{
   if (dbg) Serial << __PRETTY_FUNCTION__ << " here" << endl;
   ui.flashNone();
   ui.setState( &state_Awake );
}

void UI_DayAdvance::timeLongEvent()
{
   if (dbg) Serial << __PRETTY_FUNCTION__ << " here" << endl;
   ui.setState( &state_Awake );
}

void UI_DayAdvance::onEntry()
{
   if (dbg) Serial << __PRETTY_FUNCTION__ << " here" << endl;
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
   if (dbg) Serial << __PRETTY_FUNCTION__ << " here" << endl;
   ui.incrYear();
}

void UI_YearAdvance::downEvent()
{
   if (dbg) Serial << __PRETTY_FUNCTION__ << " here" << endl;
   ui.decrYear();
}

void UI_YearAdvance::modeEvent()
{
   if (dbg) Serial << __PRETTY_FUNCTION__ << " here" << endl;
   ui.moveToNextSetting();
   ui.setState( &state_TemperatureAdvance );
}

void UI_YearAdvance::stepEvent()
{
   if (dbg) Serial << __PRETTY_FUNCTION__ << " here" << endl;
   ui.setState( &state_TemperatureAdvance );
}

void UI_YearAdvance::setEvent()
{
   if (dbg) Serial << __PRETTY_FUNCTION__ << " here" << endl;
   ui.saveSettings(true);
   ui.setState( &state_Awake );
}

void UI_YearAdvance::clearEvent()
{
   if (dbg) Serial << __PRETTY_FUNCTION__ << " here" << endl;
   ui.flashNone();
   ui.setState( &state_Awake );
}

void UI_YearAdvance::timeLongEvent()
{
   if (dbg) Serial << __PRETTY_FUNCTION__ << " here" << endl;
   ui.setState( &state_Awake );
}

void UI_YearAdvance::onEntry()
{
   if (dbg) Serial << __PRETTY_FUNCTION__ << " here" << endl;
   ui.flashYear();
}
