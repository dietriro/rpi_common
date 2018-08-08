#include "LCD.h"

/* Suche nach I2C-Adressen */
void scan_i2c_bus(int device)
{
   int port, res;

   /* Adressbereich 7 Bit */
   for (port = 0; port < 127; port++)
   {
      if (ioctl(device, I2C_SLAVE, port) < 0)
         perror("ioctl() I2C_SLAVE failed\n");
      else
      {
         /* kann gelesen werden? */
         res = i2c_smbus_read_byte(device);
         if (res >= 0) {
           printf("i2c chip found at: %x, val = %d\n", port, res);
         }
      }
   }
}

LCD::LCD() {
   unsigned long funcs;

   /* Device oeffen */
   if ((device_ = open("/dev/i2c-1", O_RDWR)) < 0)
   {
      perror("Failed to open the i2c bus\n");
      exit(1);
   } else
      printf("# INFO #   Opened device successfully\n");

   /* Spezifizieren der Slave-Adresse -> Kommunikation frei geben */
   if (ioctl(device_, I2C_SLAVE, 0x27) < 0)
   {
     perror("Failed to acquire bus access and/or talk to slave\n");
     exit(1);
   } else
      printf("# INFO #   R/W Connection successfully established - %i \n", device_);

   // /* Abfragen, ob die I2C-Funktionen da sind */
   // if (ioctl(device_,I2C_FUNCS,&funcs) < 0)
   // {
   //    perror("ioctl() I2C_FUNCS failed");
   //    exit (1);
   // }

         int value = i2c_smbus_read_byte(device_);
   //i2c_smbus_write_byte(device, 161);

         uint8_t asd = 100;
      int result = i2c_smbus_write_byte(device_, asd);

      if( result != 0 )
      {
         char buffer[ 256 ];
         char * errorMessage = strerror_r( errno, buffer, 256 ); // get string message from errno
         printf(errorMessage);
         printf("\n");
      } else
         printf("NO ERROR!!!!! \n");

   // /* Ergebnis untersuchen */
   // if (funcs & I2C_FUNC_I2C)
   //    printf("I2C\n");
   // if (funcs & (I2C_FUNC_SMBUS_WRITE_BYTE))
   //    printf("I2C_FUNC_SMBUS_WRITE_BYTE\n");

   // /* und Bus abscannen */
   // scan_i2c_bus(device_);

   lcd_write(0x03);
   lcd_write(0x03);
   lcd_write(0x03);
   lcd_write(0x02);

   lcd_write(LCD_FUNCTIONSET | LCD_2LINE | LCD_5x8DOTS | LCD_4BITMODE);
   lcd_write(LCD_DISPLAYCONTROL | LCD_DISPLAYON);
   lcd_write(LCD_CLEARDISPLAY);
   lcd_write(LCD_ENTRYMODESET | LCD_ENTRYLEFT);
   usleep(200000);
}

// Functions

// clocks EN to latch command
void LCD::lcd_strobe(uint8_t data){
   write_cmd(data | En | LCD_BACKLIGHT);
   usleep(500);
   write_cmd(((data & ~En) | LCD_BACKLIGHT));
   usleep(100);
}

void LCD::lcd_write_four_bits(uint8_t data){
   write_cmd(data | LCD_BACKLIGHT);
   lcd_strobe(data);
}

// write a command to lcd
void LCD::lcd_write(uint8_t cmd, uint8_t mode){
   lcd_write_four_bits(mode | (cmd & 0xF0));
   lcd_write_four_bits(mode | ((cmd << 4) & 0xF0));
}

// put string function
void LCD::lcd_display_string(string input, int line){
   if (line == 1)
      lcd_write(0x80);
   else if (line == 2)
      lcd_write(0xC0);
   else if (line == 3)
      lcd_write(0x94);
   else if (line == 4)
      lcd_write(0xD4);

   uint8_t ascii = 0;
   for (int i=0;i<input.length();i++) {
      ascii = (uint8_t) input.at(i);
      lcd_write(ascii, Rs);
      printf("# VAR  #   ascii[%i] = %i \n", i, ascii);
   }
}

// clear lcd and set to home
void LCD::lcd_clear(){
   lcd_write(LCD_CLEARDISPLAY);
   lcd_write(LCD_RETURNHOME);
}


//
// I2C Operations

// Write a single command
   void LCD::write_cmd(uint8_t cmd) {
      printf("# VAR  #   dev = %i, cmd = %i \n", device_, cmd);
   
      int result = i2c_smbus_write_byte(device_, cmd);

      if( result != 0 )
      {
         char buffer[ 256 ];
         char * errorMessage = strerror_r( errno, buffer, 256 ); // get string message from errno
         printf(errorMessage);
         printf("\n");
      }
      usleep(1000);
   }

// Write a command and argument
   void LCD::write_cmd_arg(uint8_t cmd, uint8_t data) {
      i2c_smbus_write_byte_data(device_, cmd, data);
      usleep(100);
   }

// Write a block of data
   // void LCD::write_block_data(int cmd, int data) {
   //    i2c_smbus_write_block_data(device_, cmd, data);
   //    sleep(0.0001);
   // }

// Read a single byte
   int LCD::read() {
      return i2c_smbus_read_byte(device_);
   }

// Read
   int LCD::read_data(uint8_t cmd) {
      return i2c_smbus_read_byte_data(device_, cmd);
   }

// Read a block of data
   // int LCD::read_block_data(int cmd) {
   //    return i2c_smbus_read_block_data(device_, cmd);   
   // }
