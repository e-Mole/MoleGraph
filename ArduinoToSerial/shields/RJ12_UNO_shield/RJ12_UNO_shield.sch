<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE eagle SYSTEM "eagle.dtd">
<eagle version="7.4.0">
<drawing>
<settings>
<setting alwaysvectorfont="no"/>
<setting verticaltext="up"/>
</settings>
<grid distance="0.1" unitdist="inch" unit="inch" style="lines" multiple="1" display="no" altdistance="0.01" altunitdist="inch" altunit="inch"/>
<layers>
<layer number="1" name="Top" color="4" fill="1" visible="no" active="no"/>
<layer number="16" name="Bottom" color="1" fill="1" visible="no" active="no"/>
<layer number="17" name="Pads" color="2" fill="1" visible="no" active="no"/>
<layer number="18" name="Vias" color="2" fill="1" visible="no" active="no"/>
<layer number="19" name="Unrouted" color="6" fill="1" visible="no" active="no"/>
<layer number="20" name="Dimension" color="15" fill="1" visible="no" active="no"/>
<layer number="21" name="tPlace" color="7" fill="1" visible="no" active="no"/>
<layer number="22" name="bPlace" color="7" fill="1" visible="no" active="no"/>
<layer number="23" name="tOrigins" color="15" fill="1" visible="no" active="no"/>
<layer number="24" name="bOrigins" color="15" fill="1" visible="no" active="no"/>
<layer number="25" name="tNames" color="7" fill="1" visible="no" active="no"/>
<layer number="26" name="bNames" color="7" fill="1" visible="no" active="no"/>
<layer number="27" name="tValues" color="7" fill="1" visible="no" active="no"/>
<layer number="28" name="bValues" color="7" fill="1" visible="no" active="no"/>
<layer number="29" name="tStop" color="7" fill="3" visible="no" active="no"/>
<layer number="30" name="bStop" color="7" fill="6" visible="no" active="no"/>
<layer number="31" name="tCream" color="7" fill="4" visible="no" active="no"/>
<layer number="32" name="bCream" color="7" fill="5" visible="no" active="no"/>
<layer number="33" name="tFinish" color="6" fill="3" visible="no" active="no"/>
<layer number="34" name="bFinish" color="6" fill="6" visible="no" active="no"/>
<layer number="35" name="tGlue" color="7" fill="4" visible="no" active="no"/>
<layer number="36" name="bGlue" color="7" fill="5" visible="no" active="no"/>
<layer number="37" name="tTest" color="7" fill="1" visible="no" active="no"/>
<layer number="38" name="bTest" color="7" fill="1" visible="no" active="no"/>
<layer number="39" name="tKeepout" color="4" fill="11" visible="no" active="no"/>
<layer number="40" name="bKeepout" color="1" fill="11" visible="no" active="no"/>
<layer number="41" name="tRestrict" color="4" fill="10" visible="no" active="no"/>
<layer number="42" name="bRestrict" color="1" fill="10" visible="no" active="no"/>
<layer number="43" name="vRestrict" color="2" fill="10" visible="no" active="no"/>
<layer number="44" name="Drills" color="7" fill="1" visible="no" active="no"/>
<layer number="45" name="Holes" color="7" fill="1" visible="no" active="no"/>
<layer number="46" name="Milling" color="3" fill="1" visible="no" active="no"/>
<layer number="47" name="Measures" color="7" fill="1" visible="no" active="no"/>
<layer number="48" name="Document" color="7" fill="1" visible="no" active="no"/>
<layer number="49" name="Reference" color="7" fill="1" visible="no" active="no"/>
<layer number="51" name="tDocu" color="7" fill="1" visible="no" active="no"/>
<layer number="52" name="bDocu" color="7" fill="1" visible="no" active="no"/>
<layer number="90" name="Modules" color="5" fill="1" visible="yes" active="yes"/>
<layer number="91" name="Nets" color="2" fill="1" visible="yes" active="yes"/>
<layer number="92" name="Busses" color="1" fill="1" visible="yes" active="yes"/>
<layer number="93" name="Pins" color="2" fill="1" visible="no" active="yes"/>
<layer number="94" name="Symbols" color="4" fill="1" visible="yes" active="yes"/>
<layer number="95" name="Names" color="7" fill="1" visible="yes" active="yes"/>
<layer number="96" name="Values" color="7" fill="1" visible="yes" active="yes"/>
<layer number="97" name="Info" color="7" fill="1" visible="yes" active="yes"/>
<layer number="98" name="Guide" color="6" fill="1" visible="yes" active="yes"/>
<layer number="99" name="SpiceOrder" color="5" fill="1" visible="yes" active="yes"/>
</layers>
<schematic xreflabel="%F%N/%S.%C%R" xrefpart="/%S.%C%R">
<libraries>
<library name="AtoG">
<packages>
<package name="ANALOG">
<pad name="A3" x="0" y="-1.27" drill="0.8"/>
<pad name="A2" x="0" y="1.27" drill="0.8"/>
<pad name="A1" x="0" y="3.81" drill="0.8"/>
<pad name="A4" x="0" y="-3.81" drill="0.8"/>
<pad name="A5" x="0" y="-6.35" drill="0.8"/>
<pad name="A0" x="0" y="6.35" drill="0.8"/>
<wire x1="-1.27" y1="-7.62" x2="1.27" y2="-7.62" width="0.127" layer="21"/>
<wire x1="1.27" y1="-7.62" x2="1.27" y2="5.08" width="0.127" layer="21"/>
<wire x1="1.27" y1="5.08" x2="1.27" y2="7.62" width="0.127" layer="21"/>
<wire x1="1.27" y1="7.62" x2="-1.27" y2="7.62" width="0.127" layer="21"/>
<wire x1="-1.27" y1="7.62" x2="-1.27" y2="5.08" width="0.127" layer="21"/>
<wire x1="-1.27" y1="5.08" x2="-1.27" y2="-7.62" width="0.127" layer="21"/>
<wire x1="-1.27" y1="5.08" x2="1.27" y2="5.08" width="0.127" layer="21"/>
<text x="-1.27" y="-3.81" size="1.27" layer="25" rot="R90">&gt;NAME</text>
</package>
<package name="POWER">
<pad name="3V3" x="0" y="1.27" drill="0.8" rot="R90"/>
<pad name="RES" x="0" y="3.81" drill="0.8"/>
<pad name="5V(1)" x="0" y="6.35" drill="0.8"/>
<pad name="5V(2)" x="0" y="-1.27" drill="0.8"/>
<pad name="GND(1)" x="0" y="-3.81" drill="0.8"/>
<pad name="NC" x="0" y="8.89" drill="0.8"/>
<wire x1="-1.27" y1="-10.16" x2="1.27" y2="-10.16" width="0.127" layer="21"/>
<wire x1="1.27" y1="-10.16" x2="1.27" y2="7.62" width="0.127" layer="21"/>
<wire x1="1.27" y1="7.62" x2="1.27" y2="10.16" width="0.127" layer="21"/>
<wire x1="1.27" y1="10.16" x2="-1.27" y2="10.16" width="0.127" layer="21"/>
<wire x1="-1.27" y1="10.16" x2="-1.27" y2="7.62" width="0.127" layer="21"/>
<wire x1="-1.27" y1="7.62" x2="-1.27" y2="-10.16" width="0.127" layer="21"/>
<wire x1="-1.27" y1="7.62" x2="1.27" y2="7.62" width="0.127" layer="21"/>
<pad name="GBD(2)" x="0" y="-6.35" drill="0.8"/>
<pad name="VIN" x="0" y="-8.89" drill="0.8"/>
<text x="-1.27" y="-2.54" size="1.27" layer="25" rot="R90">&gt;NAME</text>
</package>
<package name="DIGITAL0-7">
<pad name="D3" x="0" y="-1.27" drill="0.8" rot="R270"/>
<pad name="D2" x="0" y="-3.81" drill="0.8" rot="R180"/>
<pad name="D1" x="0" y="-6.35" drill="0.8" rot="R180"/>
<pad name="D4" x="0" y="1.27" drill="0.8" rot="R180"/>
<pad name="D5" x="0" y="3.81" drill="0.8" rot="R180"/>
<pad name="D0" x="0" y="-8.89" drill="0.8" rot="R180"/>
<wire x1="1.27" y1="10.16" x2="-1.27" y2="10.16" width="0.127" layer="21"/>
<wire x1="-1.27" y1="10.16" x2="-1.27" y2="-7.62" width="0.127" layer="21"/>
<wire x1="-1.27" y1="-7.62" x2="-1.27" y2="-10.16" width="0.127" layer="21"/>
<wire x1="-1.27" y1="-10.16" x2="1.27" y2="-10.16" width="0.127" layer="21"/>
<wire x1="1.27" y1="-10.16" x2="1.27" y2="-7.62" width="0.127" layer="21"/>
<wire x1="1.27" y1="-7.62" x2="1.27" y2="10.16" width="0.127" layer="21"/>
<wire x1="1.27" y1="-7.62" x2="-1.27" y2="-7.62" width="0.127" layer="21"/>
<pad name="D6" x="0" y="6.35" drill="0.8" rot="R180"/>
<pad name="D7" x="0" y="8.89" drill="0.8" rot="R180"/>
<text x="-1.27" y="-2.54" size="1.27" layer="25" rot="R90">&gt;NAME</text>
</package>
<package name="DIGITAL8-SCL">
<pad name="D11" x="0" y="-3.81" drill="0.8" rot="R270"/>
<pad name="D10" x="0" y="-6.35" drill="0.8" rot="R180"/>
<pad name="D9" x="0" y="-8.89" drill="0.8" rot="R180"/>
<pad name="D12" x="0" y="-1.27" drill="0.8" rot="R180"/>
<pad name="D13" x="0" y="1.27" drill="0.8" rot="R180"/>
<pad name="D8" x="0" y="-11.43" drill="0.8" rot="R180"/>
<wire x1="1.27" y1="12.7" x2="-1.27" y2="12.7" width="0.127" layer="21"/>
<wire x1="-1.27" y1="12.7" x2="-1.27" y2="-10.16" width="0.127" layer="21"/>
<wire x1="-1.27" y1="-10.16" x2="-1.27" y2="-12.7" width="0.127" layer="21"/>
<wire x1="-1.27" y1="-12.7" x2="1.27" y2="-12.7" width="0.127" layer="21"/>
<wire x1="1.27" y1="-12.7" x2="1.27" y2="-10.16" width="0.127" layer="21"/>
<wire x1="1.27" y1="-10.16" x2="1.27" y2="12.7" width="0.127" layer="21"/>
<wire x1="1.27" y1="-10.16" x2="-1.27" y2="-10.16" width="0.127" layer="21"/>
<pad name="GND" x="0" y="3.81" drill="0.8" rot="R180"/>
<pad name="AREF" x="0" y="6.35" drill="0.8" rot="R180"/>
<pad name="SDA" x="0" y="8.89" drill="0.8" rot="R180"/>
<pad name="SCL" x="0" y="11.43" drill="0.8" rot="R180"/>
<text x="-1.27" y="-2.54" size="1.27" layer="25" rot="R90">&gt;NAME</text>
</package>
<package name="RJ12">
<wire x1="-6.605" y1="10.3" x2="6.605" y2="10.3" width="0.2032" layer="21"/>
<wire x1="-6.605" y1="-7.88" x2="-6.605" y2="10.3" width="0.2032" layer="21"/>
<wire x1="6.605" y1="10.3" x2="6.605" y2="-7.88" width="0.2032" layer="21"/>
<pad name="1" x="-3.175" y="6.35" drill="0.9" diameter="1.5"/>
<pad name="2" x="-1.905" y="8.89" drill="0.9" diameter="1.5"/>
<pad name="3" x="-0.635" y="6.35" drill="0.9" diameter="1.5"/>
<pad name="4" x="0.635" y="8.89" drill="0.9" diameter="1.5"/>
<pad name="5" x="1.905" y="6.35" drill="0.9" diameter="1.5"/>
<pad name="6" x="3.175" y="8.89" drill="0.9" diameter="1.5"/>
<text x="-6.35" y="10.795" size="1.27" layer="25">&gt;NAME</text>
<text x="-5.08" y="-3.81" size="1.27" layer="27">&gt;VALUE</text>
<hole x="-5.08" y="0" drill="3.2"/>
<hole x="5.08" y="0" drill="3.2"/>
<wire x1="-6.605" y1="-7.88" x2="6.605" y2="-7.88" width="0.2032" layer="21"/>
<text x="-4.6" y="6.3" size="1.27" layer="21" align="center">1</text>
</package>
<package name="BLUETOOTH">
<pad name="TX" x="0" y="1.27" drill="0.8"/>
<pad name="RX" x="0" y="3.81" drill="0.8"/>
<pad name="GND" x="0" y="-1.27" drill="0.8"/>
<pad name="5V" x="0" y="-3.81" drill="0.8"/>
<wire x1="-1.27" y1="-5.08" x2="1.27" y2="-5.08" width="0.127" layer="21"/>
<wire x1="1.27" y1="-5.08" x2="1.27" y2="5.08" width="0.127" layer="21"/>
<wire x1="1.27" y1="5.08" x2="-1.27" y2="5.08" width="0.127" layer="21"/>
<wire x1="-1.27" y1="5.08" x2="-1.27" y2="-5.08" width="0.127" layer="21"/>
<text x="-2.54" y="0" size="1.27" layer="25" rot="R90" align="center">&gt;NAME</text>
</package>
<package name="R_THT_SMD">
<wire x1="5.08" y1="0" x2="4.064" y2="0" width="0.6096" layer="51"/>
<wire x1="-5.08" y1="0" x2="-4.064" y2="0" width="0.6096" layer="51"/>
<wire x1="-3.175" y1="0.889" x2="-2.921" y2="1.143" width="0.1524" layer="21" curve="-90"/>
<wire x1="-3.175" y1="-0.889" x2="-2.921" y2="-1.143" width="0.1524" layer="21" curve="90"/>
<wire x1="2.921" y1="-1.143" x2="3.175" y2="-0.889" width="0.1524" layer="21" curve="90"/>
<wire x1="2.921" y1="1.143" x2="3.175" y2="0.889" width="0.1524" layer="21" curve="-90"/>
<wire x1="-3.175" y1="-0.889" x2="-3.175" y2="0.889" width="0.1524" layer="21"/>
<wire x1="-2.921" y1="1.143" x2="-2.54" y2="1.143" width="0.1524" layer="21"/>
<wire x1="-2.413" y1="1.016" x2="-2.54" y2="1.143" width="0.1524" layer="21"/>
<wire x1="-2.921" y1="-1.143" x2="-2.54" y2="-1.143" width="0.1524" layer="21"/>
<wire x1="-2.413" y1="-1.016" x2="-2.54" y2="-1.143" width="0.1524" layer="21"/>
<wire x1="2.413" y1="1.016" x2="2.54" y2="1.143" width="0.1524" layer="21"/>
<wire x1="2.413" y1="1.016" x2="-2.413" y2="1.016" width="0.1524" layer="21"/>
<wire x1="2.413" y1="-1.016" x2="2.54" y2="-1.143" width="0.1524" layer="21"/>
<wire x1="2.413" y1="-1.016" x2="-2.413" y2="-1.016" width="0.1524" layer="21"/>
<wire x1="2.921" y1="1.143" x2="2.54" y2="1.143" width="0.1524" layer="21"/>
<wire x1="2.921" y1="-1.143" x2="2.54" y2="-1.143" width="0.1524" layer="21"/>
<wire x1="3.175" y1="-0.889" x2="3.175" y2="0.889" width="0.1524" layer="21"/>
<pad name="1A" x="-5.08" y="0" drill="0.8128" shape="octagon"/>
<pad name="2A" x="5.08" y="0" drill="0.8128" shape="octagon"/>
<text x="-3.048" y="1.524" size="1.27" layer="25" ratio="10">&gt;NAME</text>
<text x="-2.2606" y="-0.635" size="1.27" layer="27" ratio="10">&gt;VALUE</text>
<rectangle x1="3.175" y1="-0.3048" x2="4.0386" y2="0.3048" layer="21"/>
<rectangle x1="-4.0386" y1="-0.3048" x2="-3.175" y2="0.3048" layer="21"/>
<wire x1="-0.41" y1="0.635" x2="0.41" y2="0.635" width="0.1524" layer="51"/>
<wire x1="-0.41" y1="-0.635" x2="0.41" y2="-0.635" width="0.1524" layer="51"/>
<wire x1="-1.973" y1="0.983" x2="1.973" y2="0.983" width="0.0508" layer="39"/>
<wire x1="1.973" y1="0.983" x2="1.973" y2="-0.983" width="0.0508" layer="39"/>
<wire x1="1.973" y1="-0.983" x2="-1.973" y2="-0.983" width="0.0508" layer="39"/>
<wire x1="-1.973" y1="-0.983" x2="-1.973" y2="0.983" width="0.0508" layer="39"/>
<smd name="1B" x="-0.95" y="0" dx="1.3" dy="1.5" layer="1"/>
<smd name="2B" x="0.95" y="0" dx="1.3" dy="1.5" layer="1"/>
<rectangle x1="0.4064" y1="-0.6985" x2="1.0564" y2="0.7015" layer="51"/>
<rectangle x1="-1.0668" y1="-0.6985" x2="-0.4168" y2="0.7015" layer="51"/>
<rectangle x1="-0.1999" y1="-0.5001" x2="0.1999" y2="0.5001" layer="35"/>
</package>
</packages>
<symbols>
<symbol name="ANALOG">
<pin name="A0" x="5.08" y="12.7" length="middle" rot="R180"/>
<pin name="A1" x="5.08" y="7.62" length="middle" rot="R180"/>
<pin name="A2" x="5.08" y="2.54" length="middle" rot="R180"/>
<pin name="A3" x="5.08" y="-2.54" length="middle" rot="R180"/>
<pin name="A4" x="5.08" y="-7.62" length="middle" rot="R180"/>
<pin name="A5" x="5.08" y="-12.7" length="middle" rot="R180"/>
<wire x1="2.54" y1="15.24" x2="2.54" y2="-15.24" width="0.254" layer="94"/>
<wire x1="2.54" y1="-15.24" x2="-2.54" y2="-15.24" width="0.254" layer="94"/>
<wire x1="-2.54" y1="-15.24" x2="-2.54" y2="15.24" width="0.254" layer="94"/>
<wire x1="-2.54" y1="15.24" x2="2.54" y2="15.24" width="0.254" layer="94"/>
<text x="-2.54" y="15.24" size="1.27" layer="94">&gt;NAME</text>
</symbol>
<symbol name="POWER">
<pin name="5V(1)" x="5.08" y="12.7" length="middle" rot="R180"/>
<pin name="RES" x="5.08" y="7.62" length="middle" rot="R180"/>
<pin name="3.3V" x="5.08" y="2.54" length="middle" rot="R180"/>
<pin name="5V(2)" x="5.08" y="-2.54" length="middle" rot="R180"/>
<pin name="GND(1)" x="5.08" y="-7.62" length="middle" rot="R180"/>
<pin name="GND(2)" x="5.08" y="-12.7" length="middle" rot="R180"/>
<wire x1="2.54" y1="20.32" x2="2.54" y2="-20.32" width="0.254" layer="94"/>
<wire x1="2.54" y1="-20.32" x2="-2.54" y2="-20.32" width="0.254" layer="94"/>
<wire x1="-2.54" y1="-20.32" x2="-2.54" y2="20.32" width="0.254" layer="94"/>
<wire x1="-2.54" y1="20.32" x2="2.54" y2="20.32" width="0.254" layer="94"/>
<pin name="NC" x="5.08" y="17.78" length="middle" rot="R180"/>
<pin name="VIN" x="5.08" y="-17.78" length="middle" rot="R180"/>
<text x="-2.54" y="20.32" size="1.27" layer="94">&gt;NAME</text>
</symbol>
<symbol name="DIGITAL0-7">
<pin name="D6(~)" x="5.08" y="12.7" length="middle" rot="R180"/>
<pin name="D5(~)" x="5.08" y="7.62" length="middle" rot="R180"/>
<pin name="D4" x="5.08" y="2.54" length="middle" rot="R180"/>
<pin name="D3(~)" x="5.08" y="-2.54" length="middle" rot="R180"/>
<pin name="D2" x="5.08" y="-7.62" length="middle" rot="R180"/>
<pin name="D1(TX)" x="5.08" y="-12.7" length="middle" rot="R180"/>
<wire x1="2.54" y1="20.32" x2="2.54" y2="-20.32" width="0.254" layer="94"/>
<wire x1="2.54" y1="-20.32" x2="-2.54" y2="-20.32" width="0.254" layer="94"/>
<wire x1="-2.54" y1="-20.32" x2="-2.54" y2="20.32" width="0.254" layer="94"/>
<wire x1="-2.54" y1="20.32" x2="2.54" y2="20.32" width="0.254" layer="94"/>
<pin name="D7" x="5.08" y="17.78" length="middle" rot="R180"/>
<pin name="D0(RX)" x="5.08" y="-17.78" length="middle" rot="R180"/>
<text x="-2.54" y="20.32" size="1.27" layer="94">&gt;NAME</text>
</symbol>
<symbol name="DIGITAL8-SCL">
<pin name="AREF" x="5.08" y="17.78" length="middle" rot="R180"/>
<pin name="D13" x="5.08" y="7.62" length="middle" rot="R180"/>
<pin name="D12" x="5.08" y="2.54" length="middle" rot="R180"/>
<pin name="D11(~)" x="5.08" y="-2.54" length="middle" rot="R180"/>
<pin name="D10(~)" x="5.08" y="-7.62" length="middle" rot="R180"/>
<pin name="D9(~)" x="5.08" y="-12.7" length="middle" rot="R180"/>
<wire x1="2.54" y1="30.48" x2="2.54" y2="-20.32" width="0.254" layer="94"/>
<wire x1="2.54" y1="-20.32" x2="-2.54" y2="-20.32" width="0.254" layer="94"/>
<wire x1="-2.54" y1="-20.32" x2="-2.54" y2="30.48" width="0.254" layer="94"/>
<wire x1="-2.54" y1="30.48" x2="2.54" y2="30.48" width="0.254" layer="94"/>
<pin name="SDA" x="5.08" y="22.86" length="middle" rot="R180"/>
<pin name="D8" x="5.08" y="-17.78" length="middle" rot="R180"/>
<pin name="SCL" x="5.08" y="27.94" length="middle" rot="R180"/>
<pin name="GND" x="5.08" y="12.7" length="middle" rot="R180"/>
<text x="-2.54" y="30.48" size="1.27" layer="94">&gt;NAME</text>
</symbol>
<symbol name="RJ12">
<wire x1="-2.921" y1="-7.62" x2="-2.286" y2="-7.62" width="0.127" layer="94"/>
<wire x1="-1.524" y1="-7.62" x2="-1.016" y2="-7.62" width="0.127" layer="94"/>
<wire x1="-0.254" y1="-7.62" x2="0.254" y2="-7.62" width="0.127" layer="94"/>
<wire x1="0.508" y1="-7.62" x2="0.762" y2="-7.62" width="0.127" layer="94"/>
<wire x1="0.762" y1="-7.62" x2="0.762" y2="-7.112" width="0.127" layer="94"/>
<wire x1="0.762" y1="7.366" x2="0.762" y2="7.874" width="0.127" layer="94"/>
<wire x1="0.762" y1="8.382" x2="0.762" y2="8.89" width="0.127" layer="94"/>
<wire x1="-2.286" y1="8.89" x2="-2.921" y2="8.89" width="0.127" layer="94"/>
<wire x1="-2.921" y1="8.89" x2="-2.921" y2="8.128" width="0.127" layer="94"/>
<wire x1="-2.921" y1="7.112" x2="-2.921" y2="5.588" width="0.127" layer="94"/>
<wire x1="-2.921" y1="4.572" x2="-2.921" y2="3.048" width="0.127" layer="94"/>
<wire x1="-2.921" y1="2.032" x2="-2.921" y2="0.508" width="0.127" layer="94"/>
<wire x1="-2.921" y1="-5.588" x2="-2.921" y2="-7.62" width="0.127" layer="94"/>
<wire x1="0.762" y1="6.096" x2="0.762" y2="6.604" width="0.127" layer="94"/>
<wire x1="0.762" y1="4.826" x2="0.762" y2="5.334" width="0.127" layer="94"/>
<wire x1="0.762" y1="3.556" x2="0.762" y2="4.064" width="0.127" layer="94"/>
<wire x1="0.762" y1="2.286" x2="0.762" y2="2.794" width="0.127" layer="94"/>
<wire x1="0.762" y1="1.016" x2="0.762" y2="1.524" width="0.127" layer="94"/>
<wire x1="-1.016" y1="8.89" x2="-1.651" y2="8.89" width="0.127" layer="94"/>
<wire x1="-1.016" y1="8.128" x2="-2.54" y2="8.128" width="0.254" layer="94"/>
<wire x1="-2.54" y1="8.128" x2="-2.54" y2="7.112" width="0.254" layer="94"/>
<wire x1="-2.54" y1="7.112" x2="-1.016" y2="7.112" width="0.254" layer="94"/>
<wire x1="-1.016" y1="5.588" x2="-2.54" y2="5.588" width="0.254" layer="94"/>
<wire x1="-2.54" y1="5.588" x2="-2.54" y2="4.572" width="0.254" layer="94"/>
<wire x1="-2.54" y1="4.572" x2="-1.016" y2="4.572" width="0.254" layer="94"/>
<wire x1="-1.016" y1="3.048" x2="-2.54" y2="3.048" width="0.254" layer="94"/>
<wire x1="-2.54" y1="3.048" x2="-2.54" y2="2.032" width="0.254" layer="94"/>
<wire x1="-2.54" y1="2.032" x2="-1.016" y2="2.032" width="0.254" layer="94"/>
<wire x1="-1.016" y1="0.508" x2="-2.54" y2="0.508" width="0.254" layer="94"/>
<wire x1="-2.54" y1="0.508" x2="-2.54" y2="-0.508" width="0.254" layer="94"/>
<wire x1="-2.54" y1="-0.508" x2="-1.016" y2="-0.508" width="0.254" layer="94"/>
<wire x1="1.651" y1="-1.524" x2="4.191" y2="-1.524" width="0.1998" layer="94"/>
<wire x1="4.191" y1="-1.524" x2="4.191" y2="0.254" width="0.1998" layer="94"/>
<wire x1="4.191" y1="0.254" x2="5.207" y2="0.254" width="0.1998" layer="94"/>
<wire x1="5.207" y1="0.254" x2="5.207" y2="2.286" width="0.1998" layer="94"/>
<wire x1="5.207" y1="2.286" x2="4.191" y2="2.286" width="0.1998" layer="94"/>
<wire x1="4.191" y1="2.286" x2="4.191" y2="4.064" width="0.1998" layer="94"/>
<wire x1="4.191" y1="4.064" x2="1.651" y2="4.064" width="0.1998" layer="94"/>
<wire x1="1.651" y1="4.064" x2="1.651" y2="2.54" width="0.1998" layer="94"/>
<wire x1="1.651" y1="2.54" x2="1.651" y2="2.032" width="0.1998" layer="94"/>
<wire x1="1.651" y1="2.032" x2="1.651" y2="1.524" width="0.1998" layer="94"/>
<wire x1="1.651" y1="1.524" x2="1.651" y2="1.016" width="0.1998" layer="94"/>
<wire x1="1.651" y1="1.016" x2="1.651" y2="0.508" width="0.1998" layer="94"/>
<wire x1="1.651" y1="0.508" x2="1.651" y2="0" width="0.1998" layer="94"/>
<wire x1="1.651" y1="0" x2="1.651" y2="-1.524" width="0.1998" layer="94"/>
<wire x1="1.651" y1="2.032" x2="2.413" y2="2.032" width="0.1998" layer="94"/>
<wire x1="1.651" y1="1.524" x2="2.413" y2="1.524" width="0.1998" layer="94"/>
<wire x1="1.651" y1="1.016" x2="2.413" y2="1.016" width="0.1998" layer="94"/>
<wire x1="1.651" y1="0.508" x2="2.413" y2="0.508" width="0.1998" layer="94"/>
<wire x1="0.762" y1="-0.254" x2="0.762" y2="0.254" width="0.127" layer="94"/>
<wire x1="0.762" y1="-6.604" x2="0.762" y2="-6.096" width="0.127" layer="94"/>
<wire x1="0.254" y1="8.89" x2="-0.381" y2="8.89" width="0.127" layer="94"/>
<wire x1="0.762" y1="-2.794" x2="0.762" y2="-2.286" width="0.127" layer="94"/>
<wire x1="0.762" y1="-4.064" x2="0.762" y2="-3.556" width="0.127" layer="94"/>
<wire x1="-1.016" y1="-2.032" x2="-2.54" y2="-2.032" width="0.254" layer="94"/>
<wire x1="-2.54" y1="-2.032" x2="-2.54" y2="-3.048" width="0.254" layer="94"/>
<wire x1="-2.54" y1="-3.048" x2="-1.016" y2="-3.048" width="0.254" layer="94"/>
<wire x1="-1.016" y1="-4.572" x2="-2.54" y2="-4.572" width="0.254" layer="94"/>
<wire x1="-2.54" y1="-4.572" x2="-2.54" y2="-5.588" width="0.254" layer="94"/>
<wire x1="-2.54" y1="-5.588" x2="-1.016" y2="-5.588" width="0.254" layer="94"/>
<wire x1="0.762" y1="-5.334" x2="0.762" y2="-4.826" width="0.127" layer="94"/>
<wire x1="0.762" y1="-1.524" x2="0.762" y2="-1.016" width="0.127" layer="94"/>
<wire x1="-2.921" y1="-0.508" x2="-2.921" y2="-2.032" width="0.127" layer="94"/>
<wire x1="-2.921" y1="-3.048" x2="-2.921" y2="-4.572" width="0.127" layer="94"/>
<wire x1="1.651" y1="2.54" x2="2.413" y2="2.54" width="0.1998" layer="94"/>
<wire x1="1.651" y1="0" x2="2.413" y2="0" width="0.1998" layer="94"/>
<text x="-2.54" y="9.144" size="1.778" layer="95">&gt;NAME</text>
<text x="2.54" y="-7.62" size="1.778" layer="96">&gt;VALUE</text>
<pin name="1" x="-5.08" y="7.62" visible="pad" length="short" direction="pas" swaplevel="1"/>
<pin name="2" x="-5.08" y="5.08" visible="pad" length="short" direction="pas" swaplevel="1"/>
<pin name="3" x="-5.08" y="2.54" visible="pad" length="short" direction="pas" swaplevel="1"/>
<pin name="4" x="-5.08" y="0" visible="pad" length="short" direction="pas" swaplevel="1"/>
<pin name="5" x="-5.08" y="-2.54" visible="pad" length="short" direction="pas" swaplevel="1"/>
<pin name="6" x="-5.08" y="-5.08" visible="pad" length="short" direction="pas" swaplevel="1"/>
</symbol>
<symbol name="BLUETOOTH">
<pin name="TX" x="-5.08" y="2.54" length="middle"/>
<pin name="RX" x="-5.08" y="7.62" length="middle"/>
<pin name="GND" x="-5.08" y="-2.54" length="middle"/>
<pin name="5V" x="-5.08" y="-7.62" length="middle"/>
<wire x1="-2.54" y1="10.16" x2="-2.54" y2="-10.16" width="0.254" layer="94"/>
<wire x1="-2.54" y1="-10.16" x2="2.54" y2="-10.16" width="0.254" layer="94"/>
<wire x1="2.54" y1="-10.16" x2="2.54" y2="10.16" width="0.254" layer="94"/>
<wire x1="2.54" y1="10.16" x2="-2.54" y2="10.16" width="0.254" layer="94"/>
<text x="-2.54" y="10.16" size="1.27" layer="94">&gt;NAME</text>
</symbol>
<symbol name="R">
<wire x1="-2.54" y1="-0.889" x2="2.54" y2="-0.889" width="0.254" layer="94"/>
<wire x1="2.54" y1="0.889" x2="-2.54" y2="0.889" width="0.254" layer="94"/>
<wire x1="2.54" y1="-0.889" x2="2.54" y2="0.889" width="0.254" layer="94"/>
<wire x1="-2.54" y1="-0.889" x2="-2.54" y2="0.889" width="0.254" layer="94"/>
<text x="-3.81" y="1.4986" size="1.778" layer="95">&gt;NAME</text>
<text x="-3.81" y="-3.302" size="1.778" layer="96">&gt;VALUE</text>
<pin name="2" x="5.08" y="0" visible="off" length="short" direction="pas" swaplevel="1" rot="R180"/>
<pin name="1" x="-5.08" y="0" visible="off" length="short" direction="pas" swaplevel="1"/>
<text x="-5.08" y="0" size="0.4064" layer="99" align="center">SpiceOrder 1</text>
<text x="5.08" y="0" size="0.4064" layer="99" align="center">SpiceOrder 2</text>
</symbol>
</symbols>
<devicesets>
<deviceset name="ANALOG">
<gates>
<gate name="G$1" symbol="ANALOG" x="0" y="0"/>
</gates>
<devices>
<device name="" package="ANALOG">
<connects>
<connect gate="G$1" pin="A0" pad="A0"/>
<connect gate="G$1" pin="A1" pad="A1"/>
<connect gate="G$1" pin="A2" pad="A2"/>
<connect gate="G$1" pin="A3" pad="A3"/>
<connect gate="G$1" pin="A4" pad="A4"/>
<connect gate="G$1" pin="A5" pad="A5"/>
</connects>
<technologies>
<technology name=""/>
</technologies>
</device>
</devices>
</deviceset>
<deviceset name="POWER">
<gates>
<gate name="G$1" symbol="POWER" x="0" y="0"/>
</gates>
<devices>
<device name="" package="POWER">
<connects>
<connect gate="G$1" pin="3.3V" pad="3V3"/>
<connect gate="G$1" pin="5V(1)" pad="5V(1)"/>
<connect gate="G$1" pin="5V(2)" pad="5V(2)"/>
<connect gate="G$1" pin="GND(1)" pad="GBD(2)"/>
<connect gate="G$1" pin="GND(2)" pad="GND(1)"/>
<connect gate="G$1" pin="NC" pad="NC"/>
<connect gate="G$1" pin="RES" pad="RES"/>
<connect gate="G$1" pin="VIN" pad="VIN"/>
</connects>
<technologies>
<technology name=""/>
</technologies>
</device>
</devices>
</deviceset>
<deviceset name="DIGITAL0-7">
<gates>
<gate name="G$1" symbol="DIGITAL0-7" x="0" y="0"/>
</gates>
<devices>
<device name="" package="DIGITAL0-7">
<connects>
<connect gate="G$1" pin="D0(RX)" pad="D0"/>
<connect gate="G$1" pin="D1(TX)" pad="D1"/>
<connect gate="G$1" pin="D2" pad="D2"/>
<connect gate="G$1" pin="D3(~)" pad="D3"/>
<connect gate="G$1" pin="D4" pad="D4"/>
<connect gate="G$1" pin="D5(~)" pad="D5"/>
<connect gate="G$1" pin="D6(~)" pad="D6"/>
<connect gate="G$1" pin="D7" pad="D7"/>
</connects>
<technologies>
<technology name=""/>
</technologies>
</device>
</devices>
</deviceset>
<deviceset name="DIGITAL8-SCL">
<gates>
<gate name="G$1" symbol="DIGITAL8-SCL" x="0" y="-5.08"/>
</gates>
<devices>
<device name="" package="DIGITAL8-SCL">
<connects>
<connect gate="G$1" pin="AREF" pad="AREF"/>
<connect gate="G$1" pin="D10(~)" pad="D10"/>
<connect gate="G$1" pin="D11(~)" pad="D11"/>
<connect gate="G$1" pin="D12" pad="D12"/>
<connect gate="G$1" pin="D13" pad="D13"/>
<connect gate="G$1" pin="D8" pad="D8"/>
<connect gate="G$1" pin="D9(~)" pad="D9"/>
<connect gate="G$1" pin="GND" pad="GND"/>
<connect gate="G$1" pin="SCL" pad="SCL"/>
<connect gate="G$1" pin="SDA" pad="SDA"/>
</connects>
<technologies>
<technology name=""/>
</technologies>
</device>
</devices>
</deviceset>
<deviceset name="RJ12">
<gates>
<gate name="G$1" symbol="RJ12" x="0" y="0"/>
</gates>
<devices>
<device name="" package="RJ12">
<connects>
<connect gate="G$1" pin="1" pad="1"/>
<connect gate="G$1" pin="2" pad="2"/>
<connect gate="G$1" pin="3" pad="3"/>
<connect gate="G$1" pin="4" pad="4"/>
<connect gate="G$1" pin="5" pad="5"/>
<connect gate="G$1" pin="6" pad="6"/>
</connects>
<technologies>
<technology name=""/>
</technologies>
</device>
</devices>
</deviceset>
<deviceset name="BLUETOOTH">
<gates>
<gate name="G$1" symbol="BLUETOOTH" x="0" y="0"/>
</gates>
<devices>
<device name="" package="BLUETOOTH">
<connects>
<connect gate="G$1" pin="5V" pad="5V"/>
<connect gate="G$1" pin="GND" pad="GND"/>
<connect gate="G$1" pin="RX" pad="RX"/>
<connect gate="G$1" pin="TX" pad="TX"/>
</connects>
<technologies>
<technology name=""/>
</technologies>
</device>
</devices>
</deviceset>
<deviceset name="R_THT_SMD">
<gates>
<gate name="G$1" symbol="R" x="0" y="0"/>
</gates>
<devices>
<device name="" package="R_THT_SMD">
<connects>
<connect gate="G$1" pin="1" pad="1A 1B"/>
<connect gate="G$1" pin="2" pad="2A 2B"/>
</connects>
<technologies>
<technology name=""/>
</technologies>
</device>
</devices>
</deviceset>
</devicesets>
</library>
</libraries>
<attributes>
</attributes>
<variantdefs>
</variantdefs>
<classes>
<class number="0" name="default" width="0" drill="0">
</class>
</classes>
<parts>
<part name="ANALOG" library="AtoG" deviceset="ANALOG" device=""/>
<part name="POWER" library="AtoG" deviceset="POWER" device=""/>
<part name="D0-D7" library="AtoG" deviceset="DIGITAL0-7" device=""/>
<part name="D8-SCL" library="AtoG" deviceset="DIGITAL8-SCL" device=""/>
<part name="CON1" library="AtoG" deviceset="RJ12" device=""/>
<part name="CON2" library="AtoG" deviceset="RJ12" device=""/>
<part name="CON3" library="AtoG" deviceset="RJ12" device=""/>
<part name="CON4" library="AtoG" deviceset="RJ12" device=""/>
<part name="U$5" library="AtoG" deviceset="BLUETOOTH" device=""/>
<part name="D0-D1" library="AtoG" deviceset="DIGITAL0-7" device=""/>
<part name="D8-SCL1" library="AtoG" deviceset="DIGITAL8-SCL" device=""/>
<part name="POWER1" library="AtoG" deviceset="POWER" device=""/>
<part name="ANALOG1" library="AtoG" deviceset="ANALOG" device=""/>
<part name="R1" library="AtoG" deviceset="R_THT_SMD" device=""/>
<part name="R2" library="AtoG" deviceset="R_THT_SMD" device=""/>
<part name="U$1" library="AtoG" deviceset="R_THT_SMD" device=""/>
<part name="U$2" library="AtoG" deviceset="R_THT_SMD" device=""/>
</parts>
<sheets>
<sheet>
<plain>
</plain>
<instances>
<instance part="ANALOG" gate="G$1" x="15.24" y="12.7"/>
<instance part="POWER" gate="G$1" x="15.24" y="50.8"/>
<instance part="D0-D7" gate="G$1" x="101.6" y="17.78"/>
<instance part="D8-SCL" gate="G$1" x="101.6" y="63.5"/>
<instance part="CON1" gate="G$1" x="-27.94" y="73.66" rot="MR0"/>
<instance part="CON2" gate="G$1" x="-27.94" y="48.26" rot="MR0"/>
<instance part="CON3" gate="G$1" x="-27.94" y="22.86" rot="MR0"/>
<instance part="CON4" gate="G$1" x="-27.94" y="-2.54" rot="MR0"/>
<instance part="U$5" gate="G$1" x="53.34" y="7.62" rot="R180"/>
<instance part="D0-D1" gate="G$1" x="119.38" y="17.78"/>
<instance part="D8-SCL1" gate="G$1" x="119.38" y="63.5"/>
<instance part="POWER1" gate="G$1" x="33.02" y="50.8"/>
<instance part="ANALOG1" gate="G$1" x="33.02" y="12.7"/>
<instance part="R1" gate="G$1" x="0" y="104.14" rot="R90"/>
<instance part="R2" gate="G$1" x="-5.08" y="104.14" rot="R90"/>
<instance part="U$1" gate="G$1" x="86.36" y="5.08"/>
<instance part="U$2" gate="G$1" x="93.98" y="-10.16" rot="R90"/>
</instances>
<busses>
</busses>
<nets>
<net name="N$1" class="0">
<segment>
<pinref part="ANALOG" gate="G$1" pin="A5"/>
<wire x1="-2.54" y1="76.2" x2="-2.54" y2="50.8" width="0.1524" layer="91"/>
<wire x1="-2.54" y1="50.8" x2="-2.54" y2="25.4" width="0.1524" layer="91"/>
<wire x1="-2.54" y1="25.4" x2="-2.54" y2="0" width="0.1524" layer="91"/>
<wire x1="20.32" y1="0" x2="-2.54" y2="0" width="0.1524" layer="91"/>
<junction x="-2.54" y="0"/>
<pinref part="ANALOG1" gate="G$1" pin="A5"/>
<wire x1="20.32" y1="0" x2="38.1" y2="0" width="0.1524" layer="91"/>
<junction x="20.32" y="0"/>
<pinref part="CON2" gate="G$1" pin="3"/>
<wire x1="-22.86" y1="50.8" x2="-2.54" y2="50.8" width="0.1524" layer="91"/>
<pinref part="CON3" gate="G$1" pin="3"/>
<wire x1="-22.86" y1="25.4" x2="-2.54" y2="25.4" width="0.1524" layer="91"/>
<pinref part="CON4" gate="G$1" pin="3"/>
<wire x1="-22.86" y1="0" x2="-2.54" y2="0" width="0.1524" layer="91"/>
<junction x="-2.54" y="25.4"/>
<junction x="-2.54" y="50.8"/>
<pinref part="CON1" gate="G$1" pin="3"/>
<wire x1="-22.86" y1="76.2" x2="-2.54" y2="76.2" width="0.1524" layer="91"/>
<pinref part="R1" gate="G$1" pin="1"/>
<junction x="-2.54" y="76.2"/>
<wire x1="0" y1="99.06" x2="0" y2="76.2" width="0.1524" layer="91"/>
<wire x1="0" y1="76.2" x2="-2.54" y2="76.2" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$2" class="0">
<segment>
<pinref part="ANALOG" gate="G$1" pin="A4"/>
<wire x1="-5.08" y1="73.66" x2="-5.08" y2="48.26" width="0.1524" layer="91"/>
<wire x1="-5.08" y1="48.26" x2="-5.08" y2="22.86" width="0.1524" layer="91"/>
<wire x1="-5.08" y1="22.86" x2="-5.08" y2="5.08" width="0.1524" layer="91"/>
<wire x1="-5.08" y1="5.08" x2="-5.08" y2="-2.54" width="0.1524" layer="91"/>
<wire x1="20.32" y1="5.08" x2="-5.08" y2="5.08" width="0.1524" layer="91"/>
<junction x="-5.08" y="5.08"/>
<pinref part="ANALOG1" gate="G$1" pin="A4"/>
<wire x1="20.32" y1="5.08" x2="38.1" y2="5.08" width="0.1524" layer="91"/>
<junction x="20.32" y="5.08"/>
<pinref part="CON1" gate="G$1" pin="4"/>
<wire x1="-5.08" y1="73.66" x2="-22.86" y2="73.66" width="0.1524" layer="91"/>
<pinref part="CON4" gate="G$1" pin="4"/>
<wire x1="-22.86" y1="-2.54" x2="-5.08" y2="-2.54" width="0.1524" layer="91"/>
<pinref part="CON3" gate="G$1" pin="4"/>
<wire x1="-22.86" y1="22.86" x2="-5.08" y2="22.86" width="0.1524" layer="91"/>
<pinref part="CON2" gate="G$1" pin="4"/>
<wire x1="-22.86" y1="48.26" x2="-5.08" y2="48.26" width="0.1524" layer="91"/>
<junction x="-5.08" y="22.86"/>
<junction x="-5.08" y="48.26"/>
<pinref part="R2" gate="G$1" pin="1"/>
<wire x1="-5.08" y1="73.66" x2="-5.08" y2="99.06" width="0.1524" layer="91"/>
<junction x="-5.08" y="73.66"/>
</segment>
</net>
<net name="N$4" class="0">
<segment>
<pinref part="ANALOG" gate="G$1" pin="A2"/>
<pinref part="ANALOG1" gate="G$1" pin="A2"/>
<wire x1="20.32" y1="15.24" x2="38.1" y2="15.24" width="0.1524" layer="91"/>
<pinref part="CON3" gate="G$1" pin="6"/>
<wire x1="20.32" y1="15.24" x2="-22.86" y2="15.24" width="0.1524" layer="91"/>
<wire x1="-22.86" y1="15.24" x2="-22.86" y2="17.78" width="0.1524" layer="91"/>
<junction x="20.32" y="15.24"/>
</segment>
</net>
<net name="N$5" class="0">
<segment>
<pinref part="ANALOG" gate="G$1" pin="A1"/>
<pinref part="ANALOG1" gate="G$1" pin="A1"/>
<wire x1="20.32" y1="20.32" x2="38.1" y2="20.32" width="0.1524" layer="91"/>
<wire x1="20.32" y1="20.32" x2="0" y2="20.32" width="0.1524" layer="91"/>
<wire x1="0" y1="20.32" x2="0" y2="43.18" width="0.1524" layer="91"/>
<pinref part="CON2" gate="G$1" pin="6"/>
<wire x1="0" y1="43.18" x2="-22.86" y2="43.18" width="0.1524" layer="91"/>
<junction x="20.32" y="20.32"/>
</segment>
</net>
<net name="N$6" class="0">
<segment>
<pinref part="ANALOG" gate="G$1" pin="A0"/>
<pinref part="ANALOG1" gate="G$1" pin="A0"/>
<wire x1="20.32" y1="25.4" x2="38.1" y2="25.4" width="0.1524" layer="91"/>
<wire x1="20.32" y1="25.4" x2="2.54" y2="25.4" width="0.1524" layer="91"/>
<wire x1="2.54" y1="25.4" x2="2.54" y2="68.58" width="0.1524" layer="91"/>
<pinref part="CON1" gate="G$1" pin="6"/>
<wire x1="2.54" y1="68.58" x2="-22.86" y2="68.58" width="0.1524" layer="91"/>
<junction x="20.32" y="25.4"/>
</segment>
</net>
<net name="GND" class="0">
<segment>
<pinref part="POWER" gate="G$1" pin="GND(1)"/>
<pinref part="U$5" gate="G$1" pin="GND"/>
<wire x1="58.42" y1="10.16" x2="60.96" y2="10.16" width="0.1524" layer="91"/>
<wire x1="60.96" y1="10.16" x2="60.96" y2="43.18" width="0.1524" layer="91"/>
<wire x1="60.96" y1="43.18" x2="38.1" y2="43.18" width="0.1524" layer="91"/>
<pinref part="POWER" gate="G$1" pin="GND(2)"/>
<wire x1="38.1" y1="43.18" x2="20.32" y2="43.18" width="0.1524" layer="91"/>
<wire x1="20.32" y1="38.1" x2="38.1" y2="38.1" width="0.1524" layer="91"/>
<wire x1="38.1" y1="38.1" x2="38.1" y2="43.18" width="0.1524" layer="91"/>
<junction x="38.1" y="43.18"/>
<pinref part="POWER1" gate="G$1" pin="GND(1)"/>
<junction x="38.1" y="43.18"/>
<pinref part="POWER1" gate="G$1" pin="GND(2)"/>
<junction x="38.1" y="38.1"/>
<pinref part="CON1" gate="G$1" pin="1"/>
<wire x1="-22.86" y1="81.28" x2="-20.32" y2="81.28" width="0.1524" layer="91"/>
<wire x1="-20.32" y1="81.28" x2="-20.32" y2="55.88" width="0.1524" layer="91"/>
<pinref part="CON4" gate="G$1" pin="1"/>
<wire x1="-20.32" y1="55.88" x2="-20.32" y2="38.1" width="0.1524" layer="91"/>
<wire x1="-20.32" y1="38.1" x2="-20.32" y2="30.48" width="0.1524" layer="91"/>
<wire x1="-20.32" y1="30.48" x2="-20.32" y2="5.08" width="0.1524" layer="91"/>
<wire x1="-20.32" y1="5.08" x2="-22.86" y2="5.08" width="0.1524" layer="91"/>
<pinref part="CON3" gate="G$1" pin="1"/>
<wire x1="-22.86" y1="30.48" x2="-20.32" y2="30.48" width="0.1524" layer="91"/>
<pinref part="CON2" gate="G$1" pin="1"/>
<wire x1="-22.86" y1="55.88" x2="-20.32" y2="55.88" width="0.1524" layer="91"/>
<wire x1="20.32" y1="38.1" x2="-20.32" y2="38.1" width="0.1524" layer="91"/>
<junction x="-20.32" y="38.1"/>
<junction x="-20.32" y="30.48"/>
<junction x="-20.32" y="55.88"/>
<pinref part="D8-SCL1" gate="G$1" pin="GND"/>
<pinref part="D8-SCL" gate="G$1" pin="GND"/>
<wire x1="106.68" y1="76.2" x2="124.46" y2="76.2" width="0.1524" layer="91"/>
<wire x1="106.68" y1="76.2" x2="60.96" y2="76.2" width="0.1524" layer="91"/>
<wire x1="60.96" y1="76.2" x2="60.96" y2="43.18" width="0.1524" layer="91"/>
<junction x="60.96" y="43.18"/>
<junction x="106.68" y="76.2"/>
<wire x1="60.96" y1="10.16" x2="60.96" y2="-15.24" width="0.1524" layer="91"/>
<wire x1="60.96" y1="-15.24" x2="93.98" y2="-15.24" width="0.1524" layer="91"/>
<junction x="60.96" y="10.16"/>
<pinref part="U$2" gate="G$1" pin="1"/>
</segment>
</net>
<net name="N$13" class="0">
<segment>
<pinref part="POWER" gate="G$1" pin="5V(1)"/>
<wire x1="20.32" y1="63.5" x2="38.1" y2="63.5" width="0.1524" layer="91"/>
<wire x1="38.1" y1="63.5" x2="58.42" y2="63.5" width="0.1524" layer="91"/>
<pinref part="U$5" gate="G$1" pin="5V"/>
<wire x1="58.42" y1="15.24" x2="58.42" y2="63.5" width="0.1524" layer="91"/>
<pinref part="POWER1" gate="G$1" pin="5V(1)"/>
<junction x="38.1" y="63.5"/>
</segment>
</net>
<net name="N$15" class="0">
<segment>
<pinref part="D0-D7" gate="G$1" pin="D0(RX)"/>
<pinref part="D0-D1" gate="G$1" pin="D0(RX)"/>
<wire x1="106.68" y1="0" x2="124.46" y2="0" width="0.1524" layer="91"/>
<pinref part="U$5" gate="G$1" pin="TX"/>
<wire x1="58.42" y1="5.08" x2="76.2" y2="5.08" width="0.1524" layer="91"/>
<wire x1="76.2" y1="5.08" x2="81.28" y2="0" width="0.1524" layer="91"/>
<wire x1="81.28" y1="0" x2="106.68" y2="0" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$16" class="0">
<segment>
<wire x1="81.28" y1="5.08" x2="76.2" y2="0" width="0.1524" layer="91"/>
<pinref part="U$5" gate="G$1" pin="RX"/>
<wire x1="76.2" y1="0" x2="58.42" y2="0" width="0.1524" layer="91"/>
<pinref part="U$1" gate="G$1" pin="1"/>
</segment>
</net>
<net name="N$18" class="0">
<segment>
<wire x1="63.5" y1="20.32" x2="63.5" y2="86.36" width="0.1524" layer="91"/>
<wire x1="-15.24" y1="86.36" x2="63.5" y2="86.36" width="0.1524" layer="91"/>
<pinref part="CON4" gate="G$1" pin="5"/>
<wire x1="-22.86" y1="-5.08" x2="-15.24" y2="-5.08" width="0.1524" layer="91"/>
<wire x1="-15.24" y1="86.36" x2="-15.24" y2="-5.08" width="0.1524" layer="91"/>
<pinref part="D0-D7" gate="G$1" pin="D4"/>
<pinref part="D0-D1" gate="G$1" pin="D4"/>
<wire x1="106.68" y1="20.32" x2="124.46" y2="20.32" width="0.1524" layer="91"/>
<wire x1="63.5" y1="20.32" x2="106.68" y2="20.32" width="0.1524" layer="91"/>
<junction x="106.68" y="20.32"/>
</segment>
</net>
<net name="N$20" class="0">
<segment>
<wire x1="-12.7" y1="88.9" x2="66.04" y2="88.9" width="0.1524" layer="91"/>
<wire x1="66.04" y1="88.9" x2="66.04" y2="15.24" width="0.1524" layer="91"/>
<pinref part="CON3" gate="G$1" pin="5"/>
<wire x1="-22.86" y1="20.32" x2="-12.7" y2="20.32" width="0.1524" layer="91"/>
<wire x1="-12.7" y1="88.9" x2="-12.7" y2="20.32" width="0.1524" layer="91"/>
<pinref part="D0-D1" gate="G$1" pin="D3(~)"/>
<pinref part="D0-D7" gate="G$1" pin="D3(~)"/>
<wire x1="106.68" y1="15.24" x2="124.46" y2="15.24" width="0.1524" layer="91"/>
<wire x1="66.04" y1="15.24" x2="106.68" y2="15.24" width="0.1524" layer="91"/>
<junction x="106.68" y="15.24"/>
</segment>
</net>
<net name="N$24" class="0">
<segment>
<pinref part="D8-SCL" gate="G$1" pin="D9(~)"/>
<pinref part="D8-SCL1" gate="G$1" pin="D9(~)"/>
<wire x1="106.68" y1="50.8" x2="124.46" y2="50.8" width="0.1524" layer="91"/>
<wire x1="-7.62" y1="93.98" x2="71.12" y2="93.98" width="0.1524" layer="91"/>
<wire x1="71.12" y1="93.98" x2="71.12" y2="50.8" width="0.1524" layer="91"/>
<wire x1="71.12" y1="50.8" x2="106.68" y2="50.8" width="0.1524" layer="91"/>
<wire x1="-7.62" y1="93.98" x2="-7.62" y2="71.12" width="0.1524" layer="91"/>
<pinref part="CON1" gate="G$1" pin="5"/>
<wire x1="-7.62" y1="71.12" x2="-22.86" y2="71.12" width="0.1524" layer="91"/>
<junction x="106.68" y="50.8"/>
</segment>
</net>
<net name="N$7" class="0">
<segment>
<pinref part="POWER" gate="G$1" pin="NC"/>
<pinref part="POWER1" gate="G$1" pin="NC"/>
<wire x1="20.32" y1="68.58" x2="38.1" y2="68.58" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$8" class="0">
<segment>
<pinref part="POWER" gate="G$1" pin="RES"/>
<pinref part="POWER1" gate="G$1" pin="RES"/>
<wire x1="20.32" y1="58.42" x2="38.1" y2="58.42" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$9" class="0">
<segment>
<pinref part="POWER" gate="G$1" pin="3.3V"/>
<pinref part="POWER1" gate="G$1" pin="3.3V"/>
<wire x1="20.32" y1="53.34" x2="38.1" y2="53.34" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$11" class="0">
<segment>
<pinref part="POWER" gate="G$1" pin="VIN"/>
<pinref part="POWER1" gate="G$1" pin="VIN"/>
<wire x1="20.32" y1="33.02" x2="38.1" y2="33.02" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$12" class="0">
<segment>
<pinref part="D8-SCL" gate="G$1" pin="SCL"/>
<pinref part="D8-SCL1" gate="G$1" pin="SCL"/>
<wire x1="106.68" y1="91.44" x2="124.46" y2="91.44" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$14" class="0">
<segment>
<pinref part="D8-SCL" gate="G$1" pin="SDA"/>
<pinref part="D8-SCL1" gate="G$1" pin="SDA"/>
<wire x1="106.68" y1="86.36" x2="124.46" y2="86.36" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$17" class="0">
<segment>
<pinref part="D8-SCL" gate="G$1" pin="AREF"/>
<pinref part="D8-SCL1" gate="G$1" pin="AREF"/>
<wire x1="106.68" y1="81.28" x2="124.46" y2="81.28" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$19" class="0">
<segment>
<pinref part="D8-SCL" gate="G$1" pin="D13"/>
<pinref part="D8-SCL1" gate="G$1" pin="D13"/>
<wire x1="106.68" y1="71.12" x2="124.46" y2="71.12" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$22" class="0">
<segment>
<pinref part="D8-SCL" gate="G$1" pin="D12"/>
<pinref part="D8-SCL1" gate="G$1" pin="D12"/>
<wire x1="106.68" y1="66.04" x2="124.46" y2="66.04" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$23" class="0">
<segment>
<pinref part="D8-SCL" gate="G$1" pin="D11(~)"/>
<pinref part="D8-SCL1" gate="G$1" pin="D11(~)"/>
<wire x1="106.68" y1="60.96" x2="124.46" y2="60.96" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$25" class="0">
<segment>
<pinref part="D8-SCL" gate="G$1" pin="D10(~)"/>
<pinref part="D8-SCL1" gate="G$1" pin="D10(~)"/>
<wire x1="106.68" y1="55.88" x2="124.46" y2="55.88" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$26" class="0">
<segment>
<pinref part="D8-SCL" gate="G$1" pin="D8"/>
<pinref part="D8-SCL1" gate="G$1" pin="D8"/>
<wire x1="106.68" y1="45.72" x2="124.46" y2="45.72" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$27" class="0">
<segment>
<pinref part="D0-D7" gate="G$1" pin="D7"/>
<pinref part="D0-D1" gate="G$1" pin="D7"/>
<wire x1="106.68" y1="35.56" x2="124.46" y2="35.56" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$29" class="0">
<segment>
<pinref part="D0-D7" gate="G$1" pin="D2"/>
<pinref part="D0-D1" gate="G$1" pin="D2"/>
<wire x1="106.68" y1="10.16" x2="124.46" y2="10.16" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$31" class="0">
<segment>
<pinref part="POWER1" gate="G$1" pin="5V(2)"/>
<pinref part="POWER" gate="G$1" pin="5V(2)"/>
<wire x1="20.32" y1="48.26" x2="38.1" y2="48.26" width="0.1524" layer="91"/>
<wire x1="20.32" y1="48.26" x2="7.62" y2="48.26" width="0.1524" layer="91"/>
<wire x1="7.62" y1="48.26" x2="7.62" y2="53.34" width="0.1524" layer="91"/>
<pinref part="CON1" gate="G$1" pin="2"/>
<wire x1="-22.86" y1="78.74" x2="-17.78" y2="78.74" width="0.1524" layer="91"/>
<wire x1="-17.78" y1="78.74" x2="-17.78" y2="53.34" width="0.1524" layer="91"/>
<pinref part="CON4" gate="G$1" pin="2"/>
<wire x1="-17.78" y1="53.34" x2="-17.78" y2="27.94" width="0.1524" layer="91"/>
<wire x1="-17.78" y1="27.94" x2="-17.78" y2="2.54" width="0.1524" layer="91"/>
<wire x1="-17.78" y1="2.54" x2="-22.86" y2="2.54" width="0.1524" layer="91"/>
<pinref part="CON2" gate="G$1" pin="2"/>
<wire x1="-22.86" y1="53.34" x2="-17.78" y2="53.34" width="0.1524" layer="91"/>
<pinref part="CON3" gate="G$1" pin="2"/>
<wire x1="-22.86" y1="27.94" x2="-17.78" y2="27.94" width="0.1524" layer="91"/>
<junction x="-17.78" y="53.34"/>
<junction x="-17.78" y="27.94"/>
<junction x="20.32" y="48.26"/>
<wire x1="7.62" y1="53.34" x2="-17.78" y2="53.34" width="0.1524" layer="91"/>
<wire x1="7.62" y1="53.34" x2="7.62" y2="111.76" width="0.1524" layer="91"/>
<pinref part="R2" gate="G$1" pin="2"/>
<wire x1="-5.08" y1="109.22" x2="-5.08" y2="111.76" width="0.1524" layer="91"/>
<pinref part="R1" gate="G$1" pin="2"/>
<wire x1="-5.08" y1="111.76" x2="0" y2="111.76" width="0.1524" layer="91"/>
<wire x1="0" y1="111.76" x2="0" y2="109.22" width="0.1524" layer="91"/>
<junction x="0" y="111.76"/>
<junction x="7.62" y="53.34"/>
<wire x1="7.62" y1="111.76" x2="0" y2="111.76" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$30" class="0">
<segment>
<pinref part="ANALOG1" gate="G$1" pin="A3"/>
<pinref part="ANALOG" gate="G$1" pin="A3"/>
<wire x1="20.32" y1="10.16" x2="38.1" y2="10.16" width="0.1524" layer="91"/>
<pinref part="CON4" gate="G$1" pin="6"/>
<wire x1="-22.86" y1="-7.62" x2="7.62" y2="-7.62" width="0.1524" layer="91"/>
<wire x1="7.62" y1="-7.62" x2="7.62" y2="10.16" width="0.1524" layer="91"/>
<wire x1="7.62" y1="10.16" x2="20.32" y2="10.16" width="0.1524" layer="91"/>
<junction x="20.32" y="10.16"/>
</segment>
</net>
<net name="N$3" class="0">
<segment>
<pinref part="CON2" gate="G$1" pin="5"/>
<wire x1="-22.86" y1="45.72" x2="-10.16" y2="45.72" width="0.1524" layer="91"/>
<pinref part="D0-D7" gate="G$1" pin="D6(~)"/>
<pinref part="D0-D1" gate="G$1" pin="D6(~)"/>
<wire x1="106.68" y1="30.48" x2="124.46" y2="30.48" width="0.1524" layer="91"/>
<wire x1="-10.16" y1="91.44" x2="68.58" y2="91.44" width="0.1524" layer="91"/>
<wire x1="68.58" y1="91.44" x2="68.58" y2="30.48" width="0.1524" layer="91"/>
<wire x1="68.58" y1="30.48" x2="106.68" y2="30.48" width="0.1524" layer="91"/>
<wire x1="-10.16" y1="45.72" x2="-10.16" y2="91.44" width="0.1524" layer="91"/>
<junction x="106.68" y="30.48"/>
</segment>
</net>
<net name="N$33" class="0">
<segment>
<pinref part="D0-D1" gate="G$1" pin="D5(~)"/>
<pinref part="D0-D7" gate="G$1" pin="D5(~)"/>
<wire x1="106.68" y1="25.4" x2="124.46" y2="25.4" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$21" class="0">
<segment>
<pinref part="D0-D1" gate="G$1" pin="D1(TX)"/>
<pinref part="D0-D7" gate="G$1" pin="D1(TX)"/>
<wire x1="106.68" y1="5.08" x2="124.46" y2="5.08" width="0.1524" layer="91"/>
<wire x1="91.44" y1="5.08" x2="93.98" y2="5.08" width="0.1524" layer="91"/>
<wire x1="93.98" y1="5.08" x2="106.68" y2="5.08" width="0.1524" layer="91"/>
<wire x1="93.98" y1="-5.08" x2="93.98" y2="5.08" width="0.1524" layer="91"/>
<junction x="93.98" y="5.08"/>
<pinref part="U$1" gate="G$1" pin="2"/>
<pinref part="U$2" gate="G$1" pin="2"/>
</segment>
</net>
</nets>
</sheet>
</sheets>
</schematic>
</drawing>
</eagle>
