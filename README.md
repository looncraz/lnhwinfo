# lnhwinfo
Purpose-built (for my machine) hardware monitoring dock.

If you want to use it... do it... just be aware you will need to customize
it for your specific machine.

Features:

Clock
	- What do you want me to say?  It's simple and trashy ;-)

Per-core thread usage monitoring
	- Shows CPU threads/logical processors in relation to their physical cores.
		... Intelligently determines which thread belongs to which core
	- Estimates sleep states and low frequency similarly to Ryzen Master
		... basically faking it, though... but I think RM is as well
	- Dynamically alter CPU scheduler based on usage
		... genuinely seeing higher boosts and scores as a result!
		... while also saving as much as 12W at idle / low load!

CyberPower UPS Monitoring
	- Piggy-back on CyberPower's Linux monitoring utility
		... updates every two seconds because CyberPower does the same

Generic Sensor Support
	- Uses lm-sensors to show whatever sensor data I desire.
		... Custom handlers per sensor type allows significant flexibility
	- Custom support for my Corsair Commander Pro
		... Modified output from OpenCorsairLink so I could use the same
		... parsing code for everything.

Current design issues I don't really care about... but you might:
	- Lazy root access
		... have to provide the root password to the application!
	- No graphical configuration
		... It's purpose built for my machine - I'd rather change code
		... than build a GUI.
	- Minimal error checking
		... If something goes wrong the only way to know will be bad.
	- Most everything is done by just executing a shell command
		... No usage of any topology libraries I have at my disposal
		... No integration of sensor reading support
		... Not even using IO to read /proc/cpuinfo... shame!
		* Benefit: super fast and easy to code!
	... there's probably more ...



