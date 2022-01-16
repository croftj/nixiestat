class OneWireDevice 
{
public:
   OneWireDevice()
   {
      memcpy(addr, 0, 8);
      iop = 0;
   }

   void initialize(OneWire *p, uint8_t *a) 
   {
      memcpy(addr, a, 8);
      iop = p;
   }

   OneWire  *iop;
   uint8_t  addr[8];

private:
protected:
};
