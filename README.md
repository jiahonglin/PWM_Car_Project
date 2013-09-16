#PWM_Car_Project

 * This is my PWM car project with STM32F4 and L298 motor controller . 
 * My shor-tenm goal is to build a car with obstacle avoidance.
 * My materials are as follow:
   - STM32F4 Discovery : http://www.st.com/st-web-ui/static/active/en/resource/technical/document/user_manual/DM00039084.pdf
   - L298 Motor Controller : http://wenku.baidu.com/view/0b1446d3b9f3f90f76c61b1e.html
   - Car similar to this one : http://goods.ruten.com.tw/item/show?21112137347372
   - SR04 Utrasonic module : http://letsmakerobots.com/node/30209

#Developer's Note

 * 2013/09/17 Implement Go() function

 * 2013/09/12  Finish 3 basic functions to control ONE motor
 
   - Go forward
   - Go backward
   - Stop the car
   
#Pin connection of STM32 and L298

      STM32           L298         Note
      
       3V              ENA         Always enable motor A
       
       3V              ENB         Always enable motor B
       
       PD6             in1         in1 control forward rotation
       
       PD8             in2         in2 control backward rotation  
       
       PD10            in3         in3 control forward rotation
       
       PD11            in4         in4 control backward rotation  
       
       GND             GND         common ground
       
#Reference

  * STM32F4 Discovery PWM : http://wiki.csie.ncku.edu.tw/embedded/PWM

  * L298 Reference Manual : http://wenku.baidu.com/view/0b1446d3b9f3f90f76c61b1e.html

  * Arduono with L298 Controller : http://coopermaa2nd.blogspot.tw/2012/09/l298n.html
