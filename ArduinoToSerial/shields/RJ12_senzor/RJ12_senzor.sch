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
</layers>
<schematic xreflabel="%F%N/%S.%C%R" xrefpart="/%S.%C%R">
<libraries>
<library name="AtoG">
<packages>
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
<package name="CON_SENSOR">
<pad name="4" x="1.7" y="2.5" drill="0.8"/>
<pad name="6" x="8.5" y="2.5" drill="0.8"/>
<pad name="2" x="-5.1" y="2.5" drill="0.8"/>
<pad name="5" x="5.1" y="-2.5" drill="0.8"/>
<pad name="3" x="-1.7" y="-2.5" drill="0.8"/>
<pad name="1" x="-8.5" y="-2.5" drill="0.8"/>
<wire x1="-11" y1="4.5" x2="-11" y2="-7.5" width="0.127" layer="21"/>
<wire x1="-11" y1="-7.5" x2="11" y2="-7.5" width="0.127" layer="21"/>
<wire x1="11" y1="-7.5" x2="11" y2="4.5" width="0.127" layer="21"/>
<wire x1="11" y1="4.5" x2="-11" y2="4.5" width="0.127" layer="21"/>
</package>
</packages>
<symbols>
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
<symbol name="CON_SENSOR">
<pin name="3" x="-5.08" y="0" length="short"/>
<pin name="4" x="-5.08" y="-2.54" length="short"/>
<pin name="5" x="-5.08" y="-5.08" length="short"/>
<pin name="6" x="-5.08" y="-7.62" length="short"/>
<pin name="2" x="-5.08" y="2.54" length="short"/>
<pin name="1" x="-5.08" y="5.08" length="short"/>
<wire x1="-2.54" y1="7.62" x2="-2.54" y2="-10.16" width="0.254" layer="94"/>
<wire x1="-2.54" y1="-10.16" x2="0" y2="-10.16" width="0.254" layer="94"/>
<wire x1="0" y1="-10.16" x2="5.08" y2="-10.16" width="0.254" layer="94"/>
<wire x1="5.08" y1="-10.16" x2="5.08" y2="7.62" width="0.254" layer="94"/>
<wire x1="5.08" y1="7.62" x2="0" y2="7.62" width="0.254" layer="94"/>
<circle x="1.778" y="-7.62" radius="0.762" width="0.254" layer="94"/>
<circle x="1.778" y="-5.08" radius="0.762" width="0.254" layer="94"/>
<circle x="1.778" y="-2.54" radius="0.762" width="0.254" layer="94"/>
<circle x="1.778" y="0" radius="0.762" width="0.254" layer="94"/>
<circle x="1.778" y="2.54" radius="0.762" width="0.254" layer="94"/>
<wire x1="0" y1="7.62" x2="-2.54" y2="7.62" width="0.254" layer="94"/>
<wire x1="0" y1="-10.16" x2="0" y2="7.62" width="0.254" layer="94"/>
<circle x="1.778" y="5.08" radius="0.762" width="0.254" layer="94"/>
</symbol>
</symbols>
<devicesets>
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
<deviceset name="CON_SENSOR">
<gates>
<gate name="G$1" symbol="CON_SENSOR" x="-2.54" y="0"/>
</gates>
<devices>
<device name="" package="CON_SENSOR">
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
<part name="U$1" library="AtoG" deviceset="RJ12" device=""/>
<part name="U$2" library="AtoG" deviceset="CON_SENSOR" device=""/>
</parts>
<sheets>
<sheet>
<plain>
</plain>
<instances>
<instance part="U$1" gate="G$1" x="20.32" y="66.04" rot="MR0"/>
<instance part="U$2" gate="G$1" x="38.1" y="68.58"/>
</instances>
<busses>
</busses>
<nets>
<net name="N$1" class="0">
<segment>
<pinref part="U$1" gate="G$1" pin="1"/>
<pinref part="U$2" gate="G$1" pin="1"/>
<wire x1="25.4" y1="73.66" x2="33.02" y2="73.66" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$2" class="0">
<segment>
<pinref part="U$1" gate="G$1" pin="2"/>
<pinref part="U$2" gate="G$1" pin="2"/>
<wire x1="25.4" y1="71.12" x2="33.02" y2="71.12" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$3" class="0">
<segment>
<pinref part="U$1" gate="G$1" pin="3"/>
<pinref part="U$2" gate="G$1" pin="3"/>
<wire x1="25.4" y1="68.58" x2="33.02" y2="68.58" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$4" class="0">
<segment>
<pinref part="U$1" gate="G$1" pin="4"/>
<pinref part="U$2" gate="G$1" pin="4"/>
<wire x1="25.4" y1="66.04" x2="33.02" y2="66.04" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$5" class="0">
<segment>
<pinref part="U$1" gate="G$1" pin="5"/>
<pinref part="U$2" gate="G$1" pin="5"/>
<wire x1="25.4" y1="63.5" x2="33.02" y2="63.5" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$6" class="0">
<segment>
<pinref part="U$1" gate="G$1" pin="6"/>
<pinref part="U$2" gate="G$1" pin="6"/>
<wire x1="25.4" y1="60.96" x2="33.02" y2="60.96" width="0.1524" layer="91"/>
</segment>
</net>
</nets>
</sheet>
</sheets>
</schematic>
</drawing>
</eagle>
