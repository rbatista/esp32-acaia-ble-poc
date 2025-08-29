# Quick Start Guide - ESP32 Acaia Scale Connection

This guide will help you quickly get started with connecting your ESP32 to an Acaia scale.

## Prerequisites

- ESP32 development board
- Acaia scale (Lunar, Pearl, Pyxis, or ProCH)
- USB cable
- ESP-IDF v5.0+ installed

## Step 1: Build and Flash

```bash
# Build the project
idf.py build

# Flash to your ESP32
idf.py flash monitor
```

## Step 2: Connect Your Scale

1. **Power on your Acaia scale**
2. **Ensure Bluetooth is enabled** on the scale
3. **Place the scale near the ESP32** (within 1-2 meters)
4. **Watch the serial output** - you should see:
   ```
   ESP32 Acaia Scale - Simple Example
   Scanning for Acaia scales...
   Found: ACAIA LUNAR
   Connected to ACAIA LUNAR!
   ```

## Step 3: Test Weight Reading

1. **Place an object on the scale**
2. **Watch the serial output** for weight updates:
   ```
   Weight: 0.0 g
   Weight: 15.2 g
   Weight: 23.7 g
   ```

## Troubleshooting

### Scale Not Found?
- Ensure the scale is powered on
- Check that Bluetooth is enabled
- Move the ESP32 closer to the scale
- Try restarting the scale

### Connection Fails?
- Make sure the scale is not connected to another device
- Try power cycling the scale
- Check that the scale name starts with "ACAIA", "PYXIS", "LUNAR", "PEARL", or "PROCH"

### Weight Not Updating?
- Ensure the scale is not in sleep mode
- Check that the scale is stable (not moving)
- Verify the scale is set to grams (not ounces)

## Next Steps

Once you have basic weight reading working, you can:

1. **Add tare functionality**:
   ```cpp
   if (scale) {
       scale->tare();
   }
   ```

2. **Add logging**:
   ```cpp
   void onLog(std::string msg) {
       Serial.printf("Scale: %s\n", msg.c_str());
   }
   scale->setLogCallback(onLog);
   ```

3. **Process weight data** (calculate flow rates, log to SD card, etc.)

## Example Output

```
ESP32 Acaia Scale - Simple Example
Scanning for Acaia scales...
Found: ACAIA LUNAR
Connected to ACAIA LUNAR!
Weight: 0.0 g
Weight: 0.0 g
Weight: 12.5 g
Weight: 18.3 g
Weight: 25.1 g
```

## Need Help?

- Check the main README.md for detailed documentation
- Monitor the serial output for error messages
- Ensure your ESP-IDF and Arduino framework are up to date

