# **Updated Example Mod for PvZ2**

# Credits

* **Brekko and Jay Krow** - original Lib Projects
* **Snowie** - help with reverse engineering addresses
* **Vi** - static substrate and arm64 hooking libs

# Main Features

* Static Cydia library! Lib loader is no more!
* 9.6 Addresses
* ARM64 support

# In-Game Features

* Adding new internal Plant IDs
* Adding new internal Zombie IDs
* Mummy Memory crash fix
* Custom dancing zombie list for Pianos
* More is going to come, when I fix ARM64 compatibility issues

### APK Integration

* Compile and copy the resulting libs to both cpu architecture folders in your APK.
* Add your lib to `smali_classes2/com/popcap/PvZ2/PvZ2GameActivity.smali`. Search for `.line 57`
* Rebuild your APK, install it, and play. Your custom code will be injected into PvZ2 as soon as the app is opened.
