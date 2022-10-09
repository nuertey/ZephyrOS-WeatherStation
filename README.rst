# ZephyrOS Temperature and Humidity Weather Station

## Description

Measure ambient temperature and humidity periodically (0.5 Hz) with the 
DHT11 sensor, and output the results to an LCD16x2 display. Result are also
written to the serial console. Subsequently, those very same results are
published via MQTT to a self-hosted MQTT Broker so that they can be eventually 
graphed and displayed on a Python dashboard.


## Execution Output Dashboard Plot

![Dashboard Plot](https://github.com/nuertey/ZephyrOS-WeatherStation/blob/main/weather_station-dashboard_plot.png?raw=true)
