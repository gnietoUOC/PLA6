# PLA6

## MQTTEnv2

Ejercicio que guarda el valor recogido de los sensores de la MKRENV y los envia por MQTT.
He creado una colección de clases para gestionar la convención Homie. Un ejemplo de los mensajes que recibiremos al suscribirnos a los topics de este dispositivo se muestran a continaución:

'''
genaro@ubuntu1904x64:~$ mosquitto_sub -u genaro -P passw0rd -h gnf-ubuntu1804 -t "Homie/#"  -v
Homie/MKR1000/$name MKR1000
Homie/MKR1000/MKRCORE/$name MKRCORE
Homie/MKR1000/MKRCORE/FreeMem/$name FreeMem
Homie/MKR1000/MKRCORE/FreeMem/$units KB
Homie/MKR1000/MKRCORE/FreeMem/$type Integer
Homie/MKR1000/MKRCORE/FreeMem/$settable False
Homie/MKR1000/MKRCORE/$properties FreeMem,LED
Homie/MKR1000/MKRCORE/LED/$name LED
Homie/MKR1000/MKRCORE/LED/$type Boolean
Homie/MKR1000/MKRCORE/LED/$settable True
Homie/MKR1000/MKRENV/$name MKRENV
Homie/MKR1000/MKRENV/Temperature/$name Temperature
Homie/MKR1000/MKRENV/Temperature/$units ºC
Homie/MKR1000/MKRENV/Temperature/$type Float
Homie/MKR1000/MKRENV/Temperature/$settable False
Homie/MKR1000/MKRENV/$properties Temperature,Humidity,Pressure,Illuminance,UVA,UVB,UV Index
Homie/MKR1000/MKRENV/Humidity/$name Humidity
Homie/MKR1000/MKRENV/Humidity/$units %
Homie/MKR1000/MKRENV/Humidity/$type Float
Homie/MKR1000/MKRENV/Humidity/$settable False
Homie/MKR1000/MKRENV/Pressure/$name Pressure
Homie/MKR1000/MKRENV/Pressure/$units kPa
Homie/MKR1000/MKRENV/Pressure/$type Float
Homie/MKR1000/MKRENV/Pressure/$settable False
Homie/MKR1000/MKRENV/Illuminance/$name Illuminance
Homie/MKR1000/MKRENV/Illuminance/$units lx
Homie/MKR1000/MKRENV/Illuminance/$type Float
Homie/MKR1000/MKRENV/Illuminance/$settable False
Homie/MKR1000/MKRENV/UVA/$name UVA
Homie/MKR1000/MKRENV/UVA/$type Float
Homie/MKR1000/MKRENV/UVA/$settable False
Homie/MKR1000/MKRENV/UVB/$name UVB
Homie/MKR1000/MKRENV/UVB/$type Float
Homie/MKR1000/MKRENV/UVB/$settable False
Homie/MKR1000/MKRENV/UV Index/$name UV Index
Homie/MKR1000/MKRENV/UV Index/$type Float
Homie/MKR1000/MKRENV/UV Index/$settable False
Homie/MKR1000/$nodes MKRCORE,MKRENV
Homie/MKR1000/$homie 3.0
Homie/MKR1000/$state ready
Homie/$nodes MKR1000
'''
