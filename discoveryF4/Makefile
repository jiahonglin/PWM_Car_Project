all: demos

libstm_build:
	$(MAKE) -C libstm/STM32F4xx_StdPeriph_Driver/build

demos:  libstm_build
	$(MAKE) -C discovery_demo
	$(MAKE) -C serialUSB
	$(MAKE) -C TIM_PWM_Output
	$(MAKE) -C GPIO_test

clean:
	$(MAKE) -C discovery_demo clean
	$(MAKE) -C serialUSB clean
	$(MAKE) -C libstm/STM32F4xx_StdPeriph_Driver/build clean
	$(MAKE) -C TIM_PWM_Output clean
	$(MAKE) -C GPIO_test clean
