# SaaS.de Tray

This software is part of the SaaS.de cloud based timerecording application build by EGOTEC GmbH.
 
<p>For further information visit:</p>
	<ul>
		<li>
			<a href="https://www.saas.de">SaaS.de</a>
		</li>
		<li>
			<a href="https://www.egotec.com">EGOTEC GmbH</a>
		</li>
	</ul>
	
## Install
1. Install QT 5.15.2
	- MSVC 2019 64-bit
	- QT WebEngine
3. Visual Studio 2019 Community
5. Visual Studio Workload Desktop development with C++
6. git clone https://github.com/EGOTEC-GmbH/SaaS.de-Tray

##Build
1. remove WebAPPCointainer.pro.user
2. Open Project with QT
3. Select compiler "MSVC 2019 64bit"
4. Build
5. Open release folder in cmd
6. windeployqt.exe .



