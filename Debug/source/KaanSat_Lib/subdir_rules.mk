################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
source/KaanSat_Lib/%.obj: ../source/KaanSat_Lib/%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccs1000/ccs/tools/compiler/ti-cgt-arm_20.2.0.LTS/bin/armcl" -mv7R4 --code_state=32 --include_path="C:/Users/Lenovo/Documents/CodeC/CC_2021_04x_v2" --include_path="C:/Users/Lenovo/Documents/CodeC/CC_2021_04x_v2/include" --include_path="C:/ti/ccs1000/ccs/tools/compiler/ti-cgt-arm_20.2.0.LTS/include" -g --diag_warning=225 --diag_wrap=off --display_error_number --enum_type=packed --abi=eabi --preproc_with_compile --preproc_dependency="source/KaanSat_Lib/$(basename $(<F)).d_raw" --obj_directory="source/KaanSat_Lib" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


