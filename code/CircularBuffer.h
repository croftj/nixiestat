#ifndef CIRCULARBUFFER_H
#define CIRCULARBUFFER_H

# include <stdio.h>
# include <stdlib.h>

#define CURSOR_COUNT 4

template <class T1, class T2>
class CircularBuffer
{
public:
   CircularBuffer()
   {
      m_headOfs = 0;
      m_tailOfs = 0;
      for (uint8_t cursor = 0; cursor < CURSOR_COUNT; cursor++) 
      {
         m_cursors[cursor] = 0;
      }
      m_buf = 0;
   }

   bool SetBufferSize(T2 size)
   {
      if ( m_buf != 0 ) 
      {
         free(m_buf);
      }
      m_bufSize = size + 1;
      m_buf = (T1*)malloc(sizeof(T1) * (m_bufSize));
      return(m_buf != 0);
   }

   void Push(T1 entry)
   {
      if ( m_buf == 0 ) 
      {
         return;
      }

      /***********************************************/
      /*   Start  by  placing the entry at the head  */
      /*   of  the buffer and increment the pointer  */
      /*   to the next location.                     */
      /***********************************************/
      m_buf[m_headOfs] = entry;
      m_headOfs++;

      /***********************************************/
      /*   If  we  hit the end of the buffer, reset  */
      /*   the head to the start of the buffer.      */
      /***********************************************/
      if ( m_headOfs >= m_bufSize ) 
      {
         m_headOfs = 0;
      }

      /***********************************************/
      /*   If  the  head reached the tail, move the  */
      /*   tail  forward  by one and loop it around  */
      /*   if needed                                 */
      /***********************************************/
      if ( m_headOfs == m_tailOfs ) 
      {
         m_tailOfs++;
         if ( m_tailOfs >= m_bufSize ) 
         {
            m_tailOfs = 0;
         }
      }

      /***********************************************/
      /*   Test  each  cursor,  if the head reached  */
      /*   any  of  them,  move  them  to  the next  */
      /*   position                                  */
      /***********************************************/
      for (uint8_t cursor = 0; cursor < CURSOR_COUNT; cursor++) 
      {
         if ( m_headOfs == m_cursors[cursor] ) 
         {
            m_cursors[cursor]++;
            if ( m_cursors[cursor] >= m_bufSize ) 
            {
               m_cursors[cursor] = 0;
            }
         }
      }
   }

   T1 Next(uint8_t cursor = 0)
   {
      T1 rv;

      if ( m_buf == 0 ) 
      {
         return(rv);
      }

      rv = m_buf[m_cursors[cursor]];

      if ( m_cursors[cursor] != m_headOfs ) 
      {
         m_cursors[cursor]++;
         if ( m_cursors[cursor] >= m_bufSize ) 
         {
            m_cursors[cursor] = 0;
         }
      }
      return(rv);
   }

   T1 Previous(uint8_t cursor = 0)
   {
      T1 rv;

      if ( m_buf == 0 ) 
      {
         return(rv);
      }
      if ( m_cursors[cursor] != m_tailOfs ) 
      {
         if ( m_cursors[cursor] == 0 ) 
         {
            m_cursors[cursor] = m_bufSize;
         }
         m_cursors[cursor]--;
      }

      rv = m_buf[m_cursors[cursor]];
      return(rv);
   }

   T2 Count()
   {
      T2 rv = 0;
      if ( m_buf == 0 ) 
      {
         return(rv);
      }

      if ( m_tailOfs <= m_headOfs ) 
      {
         rv = m_headOfs - m_tailOfs;
      }
      else 
      {
         rv = m_bufSize - m_tailOfs + m_headOfs;
      }
      return(rv);
   }

   void MoveToTail(uint8_t cursor = 0)
   {
      m_cursors[cursor] = m_tailOfs;
   }

   void MoveToHead(uint8_t cursor = 0)
   {
      if ( m_buf == 0 ) 
      {
         return;
      }

      m_cursors[cursor] = m_headOfs;
      if ( m_cursors[cursor] == 0 ) 
      {
         m_cursors[cursor] = m_bufSize;
      }
   }

   bool HasNext(uint8_t cursor = 0)
   {
      return ( m_cursors[cursor] != m_headOfs );
   }

   bool HasPrevious(uint8_t cursor = 0)
   {
      return ( m_cursors[cursor] != m_tailOfs );
   }

   T2 CurrentPosition(uint8_t cursor = 0)
   {
      return ( m_cursors[cursor] );
   }

   void SeekPosition(T2 pos, uint8_t cursor = 0)
   {
      m_cursors[cursor] = pos;
   }

   void Clear()
   {
      m_headOfs = 0;
      m_tailOfs = 0;
      for (uint8_t cursor = 0; cursor < CURSOR_COUNT; cursor++) 
      {
         m_cursors[cursor] = 0;
      }
   }      

protected:
private:
   T2    m_bufSize;
   T2    m_headOfs;
   T2    m_tailOfs;
   T2    m_cursors[CURSOR_COUNT];
   T1*   m_buf;
};

#endif
