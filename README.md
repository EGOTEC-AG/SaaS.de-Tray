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
1. c OpenSSL
https://slproweb.com/products/Win32OpenSSL.html
2. Install QT
https://info.qt.io/download-qt-for-application-development
3. Install Win 10 SDK
4. Visual Studio 2015
5. Visual Studio Build Tools
6. git clone https://github.com/EGOTECGmbH/SaaS.de-Tray.git

##Build
1. remove WebAPPCointainer.pro.user
2. Open Project with QT
3. Select compiler "MSVC 2015 32BIT"
4. Build
5. Open release folder in cmd
6. windeployqt.exe .



