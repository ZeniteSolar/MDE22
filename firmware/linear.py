full_right_pot_adc_value = 190
full_left_pot_adc_value = 950
center_pot_adc_value = 636
position_coef = 0.28763901

print(f"Adc left = {full_left_pot_adc_value} position = {full_left_pot_adc_value * position_coef:.1f}")
print(f"Adc center = {center_pot_adc_value} position = {center_pot_adc_value * position_coef:.1f}")
print(f"Adc right = {full_right_pot_adc_value} position = {full_right_pot_adc_value * position_coef:.1f}")