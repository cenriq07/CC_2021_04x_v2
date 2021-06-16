################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
source/%.obj: ../source/%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccs1031/ccs/tools/compiler/ti-cgt-arm_20.2.4.LTS/bin/armcl" -mv7R4 --code_state=32 --include_path="C:/Users/alex/Documents/Kaan-Sat/CC_2021_04x_v2" --include_path="C:/Users/alex/Documents/Kaan-Sat/CC_2021_04x_v2/SDCard" --include_path="C:/Users/alex/Documents/Kaan-Sat/CC_2021_04x_v2/fatfs" --include_path="C:/Users/alex/Documents/Kaan-Sat/CC_2021_04x_v2/include" --include_path="C:/ti/ccs1031/ccs/tools/compiler/ti-cgt-arm_20.2.4.LTS/include" --preinclude="C:/Users/alex/Documents/Kaan-Sat/CC_2021_04x_v2/fatfs/src/ff.h" -g --diag_warning=225 --diag_wrap=off --display_error_number --enum_type=packed --abi=eabi --preproc_with_compile --preproc_dependency="source/$(basename $(<F)).d_raw" --obj_directory="source" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

source/%.obj: ../source/%.asm $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccs1031/ccs/tools/compiler/ti-cgt-arm_20.2.4.LTS/bin/armcl" -mv7R4 --code_state=32 --include_path="C:/Users/alex/Documents/Kaan-Sat/CC_2021_04x_v2" --include_path="C:/Users/alex/Documents/Kaan-Sat/CC_2021_04x_v2/SDCard" --include_path="C:/Users/alex/Documents/Kaan-Sat/CC_2021_04x_v2/fatfs" --include_path="C:/Users/alex/Documents/Kaan-Sat/CC_2021_04x_v2/include" --include_path="C:/ti/ccs1031/ccs/tools/compiler/ti-cgt-arm_20.2.4.LTS/include" --preinclude="C:/Users/alex/Documents/Kaan-Sat/CC_2021_04x_v2/fatfs/src/ff.h" -g --diag_warning=225 --diag_wrap=off --display_error_number --enum_type=packed --abi=eabi --preproc_with_compile --preproc_dependency="source/$(basename $(<F)).d_raw" --obj_directory="source" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


