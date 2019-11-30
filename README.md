# ambient-calc
Calculation of ambiental values like dew-point, enthalpy for climate conditions based on values for temperature, humidity, pressure, wind and season.

  The motivation to create this library was, that I needed a replacement for the https://github.com/beegee-tokyo/DHTesp library I was using to calculate dewpoint and heatIndex, since I am about to replace all DHT22 sensors as they tend to deliver 100% RH if exposed to high moisture levels. I also missed in other similar libraries the calculation of enthalpy or the windchill.
  
   This library just performs the ambiental calculations and should work data read from any sensor. 
  
All values are expected to be expressed in SI-units https://en.wikipedia.org/wiki/International_System_of_Units
### Version: 1.0
### functions
1. dew-point: is calculated based on the ambiental temperature and relative humidity
2. enthalpy: delivers the enthalpy relative to a condition at 0°C 0%RH based on temperature, relative humidity and ambiental pressure
3. heatindex: is calculated based on temperature and relative humidity using the definition from the NOAA https://www.wpc.ncep.noaa.gov/html/heatindex_equation.shtml
4. windchill: uses temparature, humidity and wind-speed to calculate the windchill following the German definition
5. comfort-status: determines the comfort-zone for the current ambient according to the definition in ASHRAE 55-1992. The comfort thresholds vary between "Summer" and "Winter". Calling this function returns in a struct the comfortStatus as String and the % of comfort as float.
