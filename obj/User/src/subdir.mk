################################################################################
# MRS Version: 1.9.2
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../User/src/matrix.c \
../User/src/rtc.c 

OBJS += \
./User/src/matrix.o \
./User/src/rtc.o 

C_DEPS += \
./User/src/matrix.d \
./User/src/rtc.d 


# Each subdirectory must supply rules for building sources it contributes
User/src/%.o: ../User/src/%.c
	@	@	riscv-none-embed-gcc -march=rv32imacxw -mabi=ilp32 -msmall-data-limit=8 -msave-restore -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized  -g -I"C:\MounRiver\MounRiver_Studio\workspace\CH32V203G6U6\Debug" -I"C:\MounRiver\MounRiver_Studio\workspace\CH32V203G6U6\Core" -I"C:\MounRiver\MounRiver_Studio\workspace\CH32V203G6U6\User" -I"C:\MounRiver\MounRiver_Studio\workspace\CH32V203G6U6\Peripheral\inc" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@

