<%@ taglib uri="/WEB-INF/lib/reports_tld.jar" prefix="rw" %>
<%@ page language="java" import="java.io.*" errorPage="/rwerror.jsp" session="false" %>
<!--
<rw:report id="report">
<rw:objects id="objects">
<?xml version="1.0" encoding="WINDOWS-1250" ?>
<report name="tiskova_zostava" DTDVersion="9.0.2.0.10">
  <xmlSettings xmlTag="TISKOVA_ZOSTAVA" xmlPrologType="text">
  <![CDATA[<?xml version="1.0" encoding="&Encoding"?>]]>
  </xmlSettings>
  <reportHtmlEscapes>
    <afterPageHtmlEscape>
    <![CDATA[<hr size=5 noshade>
]]>
    </afterPageHtmlEscape>
    <beforeFormHtmlEscape>
    <![CDATA[<html dir=&Direction>
<body bgcolor="#ffffff">
<form method=post action="_action_">
<input name="hidden_run_parameters" type=hidden value="_hidden_">
<font color=red><!--error--></font>
<center>
<p><table border=0 cellspacing=0 cellpadding=0>
<tr>
<td><input type=submit></td>
<td width=15>
<td><input type=reset></td>
</tr>
</table>
<p><hr><p>
]]>
    </beforeFormHtmlEscape>
    <pageNavigationHtmlEscape>
    <![CDATA[<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html dir=&Direction>
<head>
<meta http-equiv="Content-Type"  content="text/html; charset=&Encoding">
<TITLE>Oracle HTML Navigator</TITLE>
<SCRIPT type="text/javascript" LANGUAGE = "JavaScript">

var jump_index = 1			// Jump to this page
var num_pages = &TotalPages			// Total number of pages
var basefilename = "&file_name"		// Base file name
var fileext = "&file_ext"		//File extension

/* jumps to "new_page" */
function new_page(form, new_page)
{
	form.reqpage.value = new_page;
	parent.frames[0].location = basefilename + "_" + new_page + "."+fileext;
}

/* go back one page */
function back(form)
{
	/* if we are not in first page */
	if (jump_index > 1)
	{
		jump_index--;
		new_page(form, jump_index);
	}
}

/* go forward one page */
function forward(form)
{
	/* if we are not in last page */
	if (jump_index < num_pages)
	{
		jump_index++;
		new_page(form, jump_index);
	}
}

/* go to first page */
function first(form)
{
	if(jump_index != 1)
	{
		jump_index = 1;
		new_page(form, jump_index);
	}
}

/* go to last page */
function last(form)
{
	if(jump_index != num_pages)
	{
		jump_index = num_pages;
		new_page(form, jump_index);
	}
}

/* go to the user specified page number */
function pagenum(form)
{
	/* sanity check */
	if (form.reqpage.value < 1)
	{
		form.reqpage.value = 1;
	}
	if (form.reqpage.value > num_pages)
	{
		form.reqpage.value = num_pages;
	}
	jump_index = form.reqpage.value;
	new_page(form, jump_index);
}
</SCRIPT>
</HEAD>

<BODY>
<FORM NAME="ThisForm" action="" onSubmit="pagenum(this); return false;">
<center><table><tr>
<td> <INPUT TYPE="button"  VALUE="<< " onClick="first(this.form)">
<td> <INPUT TYPE="button"  VALUE=" < " onClick="back(this.form)">
<td> <INPUT TYPE="button"  VALUE="Page:" onClick="pagenum(this.form)">
<td> <INPUT NAME="reqpage" VALUE="1" SIZE=2>
<td> <INPUT TYPE="button"  VALUE=" > " onClick="forward(this.form)">
<td> <INPUT TYPE="button"  VALUE=" >>" onClick="last(this.form)">
</table></center>
</FORM>
</body></html>]]>
    </pageNavigationHtmlEscape>
  </reportHtmlEscapes>
  <data>
    <dataSource name="Q_1">
      <select>
      <![CDATA[SELECT ALL CITATEL.MENO, CITATEL.PRIEZVISKO, CITATEL.ADRESA, CITATEL.LOGIN, 
CITATEL.TELEFON, CITATEL.EMAIL, REZERVACIA.DATUM, REZERVACIA.STAV, TITUL.NAZEV, 
TITUL.ROK_VYDANIA, TITUL.ISBN, TITUL.CENA, TITUL.NAKLADATELSTVO
FROM CITATEL, TITUL, REZERVACIA
WHERE ((REZERVACIA.ID_CITATELA = CITATEL.ID)
 AND (REZERVACIA.ID_TITUL = TITUL.ID)) ]]>
      </select>
      <displayInfo x="1.65002" y="1.00000" width="0.69995" height="0.19995"/>
      <group name="G_MENO">
        <displayInfo x="1.15759" y="1.94995" width="1.68494" height="1.69922"
        />
        <dataItem name="MENO" datatype="vchar2" columnOrder="11" width="25"
         defaultWidth="80000" defaultHeight="10000" columnFlags="33"
         defaultLabel="Meno">
          <dataDescriptor expression="CITATEL.MENO"
           descriptiveExpression="MENO" order="1" width="25"/>
          <dataItemPrivate adtName="" schemaName=""/>
        </dataItem>
        <dataItem name="PRIEZVISKO" datatype="vchar2" columnOrder="12"
         width="25" defaultWidth="90000" defaultHeight="10000"
         columnFlags="33" defaultLabel="Priezvisko">
          <dataDescriptor expression="CITATEL.PRIEZVISKO"
           descriptiveExpression="PRIEZVISKO" order="2" width="25"/>
          <dataItemPrivate adtName="" schemaName=""/>
        </dataItem>
        <dataItem name="ADRESA" datatype="vchar2" columnOrder="13" width="50"
         defaultWidth="250000" defaultHeight="10000" columnFlags="33"
         defaultLabel="Adresa">
          <dataDescriptor expression="CITATEL.ADRESA"
           descriptiveExpression="ADRESA" order="3" width="50"/>
          <dataItemPrivate adtName="" schemaName=""/>
        </dataItem>
        <dataItem name="LOGIN" datatype="vchar2" columnOrder="14" width="25"
         defaultWidth="110000" defaultHeight="10000" columnFlags="33"
         defaultLabel="Login">
          <dataDescriptor expression="CITATEL.LOGIN"
           descriptiveExpression="LOGIN" order="4" width="25"/>
          <dataItemPrivate adtName="" schemaName=""/>
        </dataItem>
        <dataItem name="TELEFON" datatype="vchar2" columnOrder="15" width="25"
         defaultWidth="130000" defaultHeight="10000" columnFlags="33"
         defaultLabel="Telefon">
          <dataDescriptor expression="CITATEL.TELEFON"
           descriptiveExpression="TELEFON" order="5" width="25"/>
          <dataItemPrivate adtName="" schemaName=""/>
        </dataItem>
        <dataItem name="EMAIL" datatype="vchar2" columnOrder="16" width="25"
         defaultWidth="210000" defaultHeight="10000" columnFlags="33"
         defaultLabel="Email">
          <dataDescriptor expression="CITATEL.EMAIL"
           descriptiveExpression="EMAIL" order="6" width="25"/>
          <dataItemPrivate adtName="" schemaName=""/>
        </dataItem>
        <dataItem name="DATUM" datatype="vchar2" columnOrder="17" width="50"
         defaultWidth="130000" defaultHeight="10000" columnFlags="33"
         defaultLabel="Datum">
          <dataDescriptor expression="REZERVACIA.DATUM"
           descriptiveExpression="DATUM" order="7" width="50"/>
          <dataItemPrivate adtName="" schemaName=""/>
        </dataItem>
        <dataItem name="STAV" datatype="vchar2" columnOrder="18" width="1"
         defaultWidth="40000" defaultHeight="10000" columnFlags="33"
         defaultLabel="Stav">
          <dataDescriptor expression="REZERVACIA.STAV"
           descriptiveExpression="STAV" order="8" width="1"/>
          <dataItemPrivate adtName="" schemaName=""/>
        </dataItem>
        <dataItem name="NAZEV" datatype="vchar2" columnOrder="19" width="100"
         defaultWidth="200000" defaultHeight="10000" columnFlags="33"
         defaultLabel="Nazev">
          <dataDescriptor expression="TITUL.NAZEV"
           descriptiveExpression="NAZEV" order="9" width="100"/>
          <dataItemPrivate adtName="" schemaName=""/>
        </dataItem>
        <dataItem name="ROK_VYDANIA" oracleDatatype="number" columnOrder="20"
         width="22" defaultWidth="50000" defaultHeight="10000"
         columnFlags="33" defaultLabel="Rok Vydania">
          <dataDescriptor expression="TITUL.ROK_VYDANIA"
           descriptiveExpression="ROK_VYDANIA" order="10" width="22"
           scale="-127"/>
          <dataItemPrivate adtName="" schemaName=""/>
        </dataItem>
        <dataItem name="ISBN" datatype="vchar2" columnOrder="21" width="25"
         defaultWidth="170000" defaultHeight="10000" columnFlags="33"
         defaultLabel="Isbn">
          <dataDescriptor expression="TITUL.ISBN" descriptiveExpression="ISBN"
           order="11" width="25"/>
          <dataItemPrivate adtName="" schemaName=""/>
        </dataItem>
        <dataItem name="CENA" oracleDatatype="number" columnOrder="22"
         width="22" defaultWidth="30000" defaultHeight="10000"
         columnFlags="33" defaultLabel="Cena">
          <dataDescriptor expression="TITUL.CENA" descriptiveExpression="CENA"
           order="12" width="22" scale="-127"/>
          <dataItemPrivate adtName="" schemaName=""/>
        </dataItem>
        <dataItem name="NAKLADATELSTVO" datatype="vchar2" columnOrder="23"
         width="50" defaultWidth="30000" defaultHeight="10000"
         columnFlags="33" defaultLabel="Nakladatelstvo">
          <dataDescriptor expression="TITUL.NAKLADATELSTVO"
           descriptiveExpression="NAKLADATELSTVO" order="13" width="50"/>
          <dataItemPrivate adtName="" schemaName=""/>
        </dataItem>
      </group>
    </dataSource>
    <summary name="SumCENAPerReport" source="CENA" function="sum" width="22"
     scale="-127" reset="report" compute="report" defaultWidth="50000"
     defaultHeight="10000" columnFlags="552" defaultLabel="Total:">
      <displayInfo x="0.00000" y="0.00000" width="0.00000" height="0.00000"/>
    </summary>
  </data>
  <layout>
  <section name="main">
    <body>
      <frame name="M_G_MENO_GRPFR">
        <geometryInfo x="0.00000" y="0.00000" width="6.93750" height="0.37500"
        />
        <generalLayout verticalElasticity="variable"/>
        <visualSettings fillPattern="transparent"
         fillBackgroundColor="r100g100b100"/>
        <repeatingFrame name="R_G_MENO" source="G_MENO" printDirection="down"
         minWidowRecords="1" columnMode="no">
          <geometryInfo x="0.00000" y="0.12500" width="6.93750"
           height="0.12500"/>
          <generalLayout verticalElasticity="expand"/>
          <visualSettings fillPattern="transparent"
           fillBackgroundColor="TableCell"/>
          <field name="F_MENO" source="MENO" minWidowLines="1" spacing="0"
           alignment="center">
            <font face="helvetica" size="5"/>
            <geometryInfo x="0.00000" y="0.12500" width="0.31250"
             height="0.12500"/>
            <generalLayout verticalElasticity="expand"/>
          </field>
          <field name="F_PRIEZVISKO" source="PRIEZVISKO" minWidowLines="1"
           spacing="0" alignment="center">
            <font face="helvetica" size="5"/>
            <geometryInfo x="0.31250" y="0.12500" width="0.43750"
             height="0.12500"/>
            <generalLayout verticalElasticity="expand"/>
          </field>
          <field name="F_ADRESA" source="ADRESA" minWidowLines="1" spacing="0"
           alignment="center">
            <font face="helvetica" size="5"/>
            <geometryInfo x="0.75000" y="0.12500" width="0.93750"
             height="0.12500"/>
            <generalLayout verticalElasticity="expand"/>
          </field>
          <field name="F_LOGIN" source="LOGIN" minWidowLines="1" spacing="0"
           alignment="center">
            <font face="helvetica" size="5"/>
            <geometryInfo x="1.68750" y="0.12500" width="0.37500"
             height="0.12500"/>
            <generalLayout verticalElasticity="expand"/>
          </field>
          <field name="F_TELEFON" source="TELEFON" minWidowLines="1"
           spacing="0" alignment="center">
            <font face="helvetica" size="5"/>
            <geometryInfo x="2.06250" y="0.12500" width="0.50000"
             height="0.12500"/>
            <generalLayout verticalElasticity="expand"/>
          </field>
          <field name="F_EMAIL" source="EMAIL" minWidowLines="1" spacing="0"
           alignment="center">
            <font face="helvetica" size="5"/>
            <geometryInfo x="2.56250" y="0.12500" width="0.75000"
             height="0.12500"/>
            <generalLayout verticalElasticity="expand"/>
          </field>
          <field name="F_DATUM" source="DATUM" minWidowLines="1" spacing="0"
           alignment="center">
            <font face="helvetica" size="5"/>
            <geometryInfo x="3.31250" y="0.12500" width="0.50000"
             height="0.12500"/>
            <generalLayout verticalElasticity="expand"/>
          </field>
          <field name="F_STAV" source="STAV" minWidowLines="1" spacing="0"
           alignment="center">
            <font face="helvetica" size="5"/>
            <geometryInfo x="3.81250" y="0.12500" width="0.18750"
             height="0.12500"/>
            <generalLayout verticalElasticity="expand"/>
          </field>
          <field name="F_NAZEV" source="NAZEV" minWidowLines="1" spacing="0"
           alignment="center">
            <font face="helvetica" size="5"/>
            <geometryInfo x="4.00000" y="0.12500" width="0.75000"
             height="0.12500"/>
            <generalLayout verticalElasticity="expand"/>
          </field>
          <field name="F_ROK_VYDANIA" source="ROK_VYDANIA" minWidowLines="1"
           spacing="0" alignment="center">
            <font face="helvetica" size="5"/>
            <geometryInfo x="4.75000" y="0.12500" width="0.43750"
             height="0.12500"/>
            <generalLayout verticalElasticity="expand"/>
          </field>
          <field name="F_ISBN" source="ISBN" minWidowLines="1" spacing="0"
           alignment="center">
            <font face="helvetica" size="5"/>
            <geometryInfo x="5.18750" y="0.12500" width="0.62500"
             height="0.12500"/>
            <generalLayout verticalElasticity="expand"/>
          </field>
          <field name="F_CENA" source="CENA" minWidowLines="1" spacing="0"
           alignment="center">
            <font face="helvetica" size="5"/>
            <geometryInfo x="5.81250" y="0.12500" width="0.56250"
             height="0.12500"/>
            <generalLayout verticalElasticity="expand"/>
          </field>
          <field name="F_NAKLADATELSTVO" source="NAKLADATELSTVO"
           minWidowLines="1" spacing="0" alignment="center">
            <font face="helvetica" size="5"/>
            <geometryInfo x="6.37500" y="0.12500" width="0.56250"
             height="0.12500"/>
            <generalLayout verticalElasticity="expand"/>
          </field>
        </repeatingFrame>
        <frame name="M_G_MENO_FTR">
          <geometryInfo x="0.00000" y="0.25000" width="6.93750"
           height="0.12500"/>
          <advancedLayout printObjectOnPage="lastPage"
           basePrintingOn="anchoringObject"/>
          <visualSettings fillPattern="transparent"
           fillBackgroundColor="Totals"/>
          <field name="F_SumCENAPerReport" source="SumCENAPerReport"
           minWidowLines="1" spacing="0" alignment="center">
            <font face="helvetica" size="5" bold="yes"/>
            <geometryInfo x="5.81250" y="0.25000" width="0.56250"
             height="0.12500"/>
            <generalLayout verticalElasticity="expand"/>
          </field>
          <text name="B_Total_" minWidowLines="1">
            <textSettings spacing="0"/>
            <geometryInfo x="0.00000" y="0.25000" width="0.25000"
             height="0.12500"/>
            <textSegment>
              <font face="helvetica" size="5" bold="yes"/>
              <string>
              <![CDATA[Total:]]>
              </string>
            </textSegment>
          </text>
        </frame>
        <frame name="M_G_MENO_HDR">
          <geometryInfo x="0.00000" y="0.00000" width="6.93750"
           height="0.12500"/>
          <advancedLayout printObjectOnPage="allPage"
           basePrintingOn="anchoringObject"/>
          <visualSettings fillPattern="transparent"
           fillBackgroundColor="TableHeading"/>
          <text name="B_MENO" minWidowLines="1">
            <textSettings justify="center" spacing="0"/>
            <geometryInfo x="0.00000" y="0.00000" width="0.31250"
             height="0.12500"/>
            <visualSettings fillBackgroundColor="r25g50b75"/>
            <textSegment>
              <font face="helvetica" size="5" bold="yes"
               textColor="r100g100b100"/>
              <string>
              <![CDATA[Meno]]>
              </string>
            </textSegment>
          </text>
          <text name="B_PRIEZVISKO" minWidowLines="1">
            <textSettings justify="center" spacing="0"/>
            <geometryInfo x="0.31250" y="0.00000" width="0.43750"
             height="0.12500"/>
            <visualSettings fillBackgroundColor="r25g50b75"/>
            <textSegment>
              <font face="helvetica" size="5" bold="yes"
               textColor="r100g100b100"/>
              <string>
              <![CDATA[Priezvisko]]>
              </string>
            </textSegment>
          </text>
          <text name="B_ADRESA" minWidowLines="1">
            <textSettings justify="center" spacing="0"/>
            <geometryInfo x="0.75000" y="0.00000" width="0.93750"
             height="0.12500"/>
            <visualSettings fillBackgroundColor="r25g50b75"/>
            <textSegment>
              <font face="helvetica" size="5" bold="yes"
               textColor="r100g100b100"/>
              <string>
              <![CDATA[Adresa]]>
              </string>
            </textSegment>
          </text>
          <text name="B_LOGIN" minWidowLines="1">
            <textSettings justify="center" spacing="0"/>
            <geometryInfo x="1.68750" y="0.00000" width="0.37500"
             height="0.12500"/>
            <visualSettings fillBackgroundColor="r25g50b75"/>
            <textSegment>
              <font face="helvetica" size="5" bold="yes"
               textColor="r100g100b100"/>
              <string>
              <![CDATA[Login]]>
              </string>
            </textSegment>
          </text>
          <text name="B_TELEFON" minWidowLines="1">
            <textSettings justify="center" spacing="0"/>
            <geometryInfo x="2.06250" y="0.00000" width="0.50000"
             height="0.12500"/>
            <visualSettings fillBackgroundColor="r25g50b75"/>
            <textSegment>
              <font face="helvetica" size="5" bold="yes"
               textColor="r100g100b100"/>
              <string>
              <![CDATA[Telefon]]>
              </string>
            </textSegment>
          </text>
          <text name="B_EMAIL" minWidowLines="1">
            <textSettings justify="center" spacing="0"/>
            <geometryInfo x="2.56250" y="0.00000" width="0.75000"
             height="0.12500"/>
            <visualSettings fillBackgroundColor="r25g50b75"/>
            <textSegment>
              <font face="helvetica" size="5" bold="yes"
               textColor="r100g100b100"/>
              <string>
              <![CDATA[Email]]>
              </string>
            </textSegment>
          </text>
          <text name="B_DATUM" minWidowLines="1">
            <textSettings justify="center" spacing="0"/>
            <geometryInfo x="3.31250" y="0.00000" width="0.50000"
             height="0.12500"/>
            <visualSettings fillBackgroundColor="r25g50b75"/>
            <textSegment>
              <font face="helvetica" size="5" bold="yes"
               textColor="r100g100b100"/>
              <string>
              <![CDATA[Datum]]>
              </string>
            </textSegment>
          </text>
          <text name="B_STAV" minWidowLines="1">
            <textSettings justify="center" spacing="0"/>
            <geometryInfo x="3.81250" y="0.00000" width="0.18750"
             height="0.12500"/>
            <visualSettings fillBackgroundColor="r25g50b75"/>
            <textSegment>
              <font face="helvetica" size="5" bold="yes"
               textColor="r100g100b100"/>
              <string>
              <![CDATA[Stav]]>
              </string>
            </textSegment>
          </text>
          <text name="B_NAZEV" minWidowLines="1">
            <textSettings justify="center" spacing="0"/>
            <geometryInfo x="4.00000" y="0.00000" width="0.75000"
             height="0.12500"/>
            <visualSettings fillBackgroundColor="r25g50b75"/>
            <textSegment>
              <font face="helvetica" size="5" bold="yes"
               textColor="r100g100b100"/>
              <string>
              <![CDATA[Nazev]]>
              </string>
            </textSegment>
          </text>
          <text name="B_ROK_VYDANIA" minWidowLines="1">
            <textSettings justify="center" spacing="0"/>
            <geometryInfo x="4.75000" y="0.00000" width="0.43750"
             height="0.12500"/>
            <visualSettings fillBackgroundColor="TableHeading"/>
            <textSegment>
              <font face="helvetica" size="5" bold="yes"
               textColor="r100g100b100"/>
              <string>
              <![CDATA[Rok Vydania]]>
              </string>
            </textSegment>
          </text>
          <text name="B_ISBN" minWidowLines="1">
            <textSettings justify="center" spacing="0"/>
            <geometryInfo x="5.18750" y="0.00000" width="0.62500"
             height="0.12500"/>
            <visualSettings fillBackgroundColor="r25g50b75"/>
            <textSegment>
              <font face="helvetica" size="5" bold="yes"
               textColor="r100g100b100"/>
              <string>
              <![CDATA[Isbn]]>
              </string>
            </textSegment>
          </text>
          <text name="B_CENA" minWidowLines="1">
            <textSettings justify="center" spacing="0"/>
            <geometryInfo x="5.81250" y="0.00000" width="0.56250"
             height="0.12500"/>
            <visualSettings fillBackgroundColor="TableHeading"/>
            <textSegment>
              <font face="helvetica" size="5" bold="yes"
               textColor="r100g100b100"/>
              <string>
              <![CDATA[Cena]]>
              </string>
            </textSegment>
          </text>
          <text name="B_NAKLADATELSTVO" minWidowLines="1">
            <textSettings justify="center" spacing="0"/>
            <geometryInfo x="6.37500" y="0.00000" width="0.56250"
             height="0.12500"/>
            <visualSettings fillBackgroundColor="r25g50b75"/>
            <textSegment>
              <font face="helvetica" size="5" bold="yes"
               textColor="r100g100b100"/>
              <string>
              <![CDATA[Nakladatelstvo]]>
              </string>
            </textSegment>
          </text>
        </frame>
      </frame>
    </body>
    <margin>
      <text name="B_OR$BODY_SECTION" minWidowLines="1">
        <textSettings justify="center" spacing="0"/>
        <geometryInfo x="3.16650" y="0.25000" width="1.70850" height="0.68750"
        />
        <textSegment>
          <font face="Times New Roman" size="16" bold="yes"/>
          <string>
          <![CDATA[Knihovna]]>
          </string>
        </textSegment>
      </text>
      <image name="B_1_SEC2" templateSection="main">
        <geometryInfo x="0.50000" y="0.43750" width="1.40625" height="0.37500"
        />
        <visualSettings fillPattern="transparent" fillBackgroundColor="black"
         linePattern="transparent" lineBackgroundColor="black"/>
        <points>
          <point x="0.00000" y="0.00000"/>
          <point x="1.40625" y="0.37500"/>
          <point x="0.50000" y="0.43750"/>
          <point x="1.40625" y="0.37500"/>
        </points>
        <binaryData encoding="hexidecimal" dataId="image.B_1_SEC2">
          
74946483 93167800 42007F00 00FFFFFF 183B180E CE0E3076 300C9D0C 1A6C1A98
8B98FE6F FE7FAF7F 0B0D0B8A BC8A19DB 198CED8C 0D3E0D7E 1F7E991C 998D7E8D
8B4D8B31 1731260A 26257925 17AA1723 482324D8 2432A732 97EA97B0 C6B0965A
96A388A3 00000000 00000000 00000000 00000000 00000000 00000000 00000000
00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000
00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000
00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000
00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000
00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000
00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000
00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000
00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000
00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000
00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000
00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000
00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000
00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000
00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000
00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000
00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000
00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000
00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000
00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000
00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000
00000000 00000000 00000000 00129F40 00000000 00C20000 00007800 42000080
FF003080 C1840B0A 1C388031 A2C58C01 1668B081 6084C10A 18305868 8133E043
08000874 0020A061 8F820A0C 143040A3 E7400408 0E8C5912 90E7C69B 031C1859
8317E189 830804EC F90438A0 51811A06 2859B080 A18B9C27 1E2070B2 96A0C794
2B0ECC68 C15829C9 8614BA7D FA8350A0 05050508 749A2091 87460855 563D1845
D36CC187 5B1E15FA B0626500 BC7DBA85 406080E3 00070203 C5F870C0 A2020112
921CBBF6 7A2EC9B2 0BEA0C70 20237307 C20D0C75 A031D76F D1DF35C2 00805090
89CD8C43 18258AF1 D53146A8 63E284BA 76D5EA44 D23DBA40 98212821 06AC2983
05202A5B A9600467 36145BE1 D0B0C4EE 1103148E 5B723EE8 C102E630 5AE590C4
FF4A0B2F 04508830 0387C20F B1381F99 41E24284 021C67EC 5E808371 450F1313
148DB292 010C0810 CD540018 108D6200 2810C747 0218F080 02121060 02809505
51C00A1A D670060A 1E400046 8A1E004D 97491288 00400304 0724D086 6974B023
810810E6 5C840EF4 F0E08F25 E8902E46 3E08E0AC 824A10FE DF461C88 F2EA4218
68904104 1C203263 083D4030 C50E1530 A1AA7D26 00942001 18C801CA 4F363893
1F8B3228 A023814A 09862081 49003267 583169A1 21002610 A12B8F19 A842A747
0C196F1E 4E2EA480 C20802F7 26AA9562 A9E1999E 3A2A000F 823A100D 97587A68
B790AA12 5031CD44 1C7826A6 8276F538 EA450028 52220C06 79E1D9A4 0640D823
FFA98228 B6AE8000 30C02CAE 5D7A576E 474EA825 200326C5 45E00462 9AC52E4E
AE085220 06821871 20070038 1C24080A 76B21EA4 0E205B6F 8E523816 A65B4EA8
931C9A12 9990C886 CD8B1DE7 55AE74BC 600DCA4A 59E10F00 38A2D4B6 1E74E045
80D67BF2 120EFE08 B08B583E 884E92B9 723522E8 A000D9D7 60540419 FE6BA8A6
E4203386 04107F23 84D5CB33 27C23629 E8E59E1B 0522E00C D2E4CB66 A008786C
200F2210 3264BC4D F90CA0A9 CA9A7179 BC6238F0 81CE0CFC BF20CB25 7C302191
4DFA300B A2C23903 E8870EB8 70C8865E E9F2149B 9AECCA6C BDCA7401 C0C24A20
60F5926B C85081C5 23C8C06B BD8E8830 CB79E29B F6F7817D 50657005 18404596
FFD3042C B7EEC0F6 FCF31204 0C3B78A3 DF1D1090 C9D10C30 1CEA4B6B 7420228D
5FED07F5 0100175C 20E20C29 C73E4704 1BF16798 3B4019B7 4C0A2054 A74C083E
2F67421A 0000A008 433BE3E7 94065A5B A75AD228 1EEA4AB3 4A067603 14608BB3
891C60AB AF930F40 9A30059F 08E6FA43 4F5BA83E 49001ECF 228E66CA 04AFE7DE
F47D31F4 937FF063 F0EB7BF1 95B4060A B82181D7 0584F7DE F26B835E 80612EAE
FA488FD8 83140810 200404AE 01080810 065D2080 86C11022 0300FE16 0410410C
AA10600C 6370880C 403A7200 40E7D210 404441E4 E08404C9 410808DF 301210FB
7F1160C3 0B489318 08707524 2C00C20C 06D1C219 20E18389 598A0758 F1930CE0
FF3001C3 00C10C28 824C9C30 A08F1100 53080859 2005D0D4 88047319 0756E118
08B5E00F 2C361E46 8860C706 FFA4423C 70257C10 F2C20030 210B0440 2E4E7880
43E09039 32087117 13000B17 04296A8B 29312309 68C4CF84 40A88B4C 70205C18
16C7066F 000E0C70 611D58D5 C716F0F3 2D6C706A EAC847C4 EC200F74 08D3AB17
52A14504 11100E74 3EC20E24 F4434205 46898440 A10AC6F1 1F3A2210 919C70AD
62294D9E 06008E84 5940046A 00E9481A 65EE8394 32AD0752 808712E4 AB4A0865
220548B2 184EB0BB 453CA5E5 01728BC5 98E29E87 CA6044E2 18F11E3A 05010584
ED142DD4 34416A74 E68F1140 0FEAA980 C15E1132 94DCE265 29086B14 2220977B
FFDCF847 0DE8C49F 7683F391 41804121 28D7C04A 74259AE4 004000A9 32C842EF
C329EC71 6E252900 07084090 A41DE2A5 151A93CC 9C10389E 5400860B 00508386
8079957C FE1211B9 4440A0C3 93A4ACA5 244FE99A B45AB947 9804BD08 1DF97D4A
22949A85 BC9F105E 7803D75E F466AD05 95671D83 E0831A10 47840C86 91430060
84081C29 902D68A3 41C9E111 7A74C9AF 2485EA29 6AFCC5A6 D298A194 88205EE8
04CFACF0 9B9065F8 8E278A90 D8465F08 2D648921 07490006 A6949CAF 243A2235
5ACE4DA0 3477904D 008AF848 87D856D2 B8886C70 E44D4AD0 5BC69D40 BC6DA52F
E208A252 0E206198 0880854B 1B5C4F36 467CA215 C9896F58 49DCCE30 12AC9DCB
44A53D68 C0842120 400110D7 6A1B10AB 5EDAF630 0908A3A0 04A96C5E 96E6BE8F
3C4EA002 4B8F47E2 47B8B9CD 10A37BB8 BC56E6D2 5AB7BDEE A77FBB0E D0FA877C
B4E51F60 4000B3DC DCDCDCDC DCDCDCDC DCDCDCDC DCDCDCDC DCDCDCDC DCDCDCDC
DCDCDCDC DCDCDCDC DCDCDCDC DCDCDCDC DCDCDCDC DCDCDCDC DCDCDCDC DCDCDCDC
DCDCDCDC DCDCDCDC DCDCDCDC DCDCDCDC DCDCDCDC DCDCDCDC DCDCDCDC DCDCDCDC
DCDCDCDC DCDCDCDC DCDCDCDC DCDCDCDC DCDCDCDC DCDCDCDC DCDCDCDC DCDCDCDC
DCDCDCDC DCDCDCDC DCDCDCDC DCDCDCDC DCDCDCDC DCDCDCDC DCDCDCDC DCDCDCDC
DCDCDCDC DCDCDCDC DCDCDCDC DCDCDCDC DCDCDCDC DCDCDCDC DCDCDCDC DCDCDCDC
DCDCDCDC DCDCDCDC DCDCDCDC DCDCDCDC DCDCDCDC DCDCDCDC DCDCDCDC DCDCDCDC
DCDCDCDC DCDCDCDC DCDCDCDC DCDCDCDC DCDCDCDC DCDCDCDC DCDCDCDC DCDCDCDC
DCDCDCDC DCDCDCDC DCDCDCDC DCDCDCDC DCDCDCDC DCDCDCDC DCDCDCDC DCDCDCDC
DCDCDCDC DCDCDCDC DCDCDCDC DCDCDCDC DCDCDCDC DCDCDCDC DCDCDCDC DCDCDCDC
DCDCDCDC DCDCDCDC DCDCDCDC DCDCDCDC DCDCDCDC DCDCDCDC DCDCDCDC DCDCDCDC
DCDCDCDC DCDCDCDC DCDCDCDC DCDCDCDC DCDCDCDC DCDCDCDC DCDCDCDC DCDCDCDC
DCDCDCDC DCDCDCDC DCDCDCDC DCDCDCDC DCDCDCDC DCDCDCDC DCDCDCDC DCDCDCDC
DCDCDCDC DCDCDCDC DCDCDCDC DCDCDCDC DCDCDCDC DCDCDCDC DCDCDCDC DCDCDCDC
DCDCDCDC DCDCDCDC DCDCDCDC DCDCDCDC DCDCDCDC DCDCDCDC DCDCDCDC DCDCDCDC
DCDCDCDC DCDCDCDC DCDCDCDC DCDCDCDC DCDCDCDC DCDCDCDC DCDCDCDC DCDCDCDC
DCDCDCDC DCDCDCDC DCDCDCDC DCDCDCDC DCDCDCDC DCDCDCDC DCDCDCDC DCDCDCDC
DCDCDCDC DCDCDCDC DCDCDCDC DCDCDCDC DCDCDCDC DCDCDCDC DCDCDCDC DCDCDCDC
DCDCDCDC DCDCDCDC DCDCDCDC DCDCDCDC DCDCDCDC DCDCDCDC DCDCDCDC DCDCDCDC
DCDCDCDC DCDCDCDC DCDCDCDC DCDCDCDC DCDCDCDC DCDCDCDC DCDCDCDC DCDCDCDC
DCDCDCDC DCDCDCDC DCDCDCDC DCDCDCDC DCDCDCDC DCDCDCDC DCDCDCDC DCDCDCDC
DCDCDCDC DCDCDCDC DCDCDCDC DCDCDCDC DCDCDCDC DCDCDCDC DCDCDCDC DCDCDCDC
DCDCDCDC DCDCDCDC DCDCDCDC DCDCDCDC DCDCDCDC DCDCDCDC DCDCDCDC DCDCDCDC
DCDCDCDC DCDCDCDC DCDCDCDC DCDCDCDC DCDCDCDC DCDCDCDC DCDCDCDC DCDCDCDC
DCDCDCDC DCDCDCDC DCDCDCDC DCDCDCDC DCDCDCDC DCDCDCDC DCDCDCDC DCDCDCDC
DCDCDCDC DCDCDCDC DCDCDCDC DCDCDCDC DCDCDCDC DCDCDCDC DCDCDCDC DCDCDCDC
DCDCDCDC DCDCDCDC DCDCDCDC DCDCDCDC DCDCDCDC DCDCDCDC DCDCDCDC DCDCDCDC
DCDCDCDC DCDCDCDC DCDCDCDC DCDCDCDC DCDCDCDC DCDCDCDC DCDCDCDC DCDCDCDC
DCDCDCDC DCDCDCDC DCDCDCDC DCDCDFDF
        </binaryData>
      </image>
    </margin>
  </section>
  </layout>
  <colorPalette>
    <color index="190" displayName="TextColor" value="#000000"/>
    <color index="191" displayName="TableHeading" value="#046804"/>
    <color index="192" displayName="TableCell" value="#c0d9c0"/>
    <color index="193" displayName="Totals" value="#81b381"/>
  </colorPalette>
  <reportPrivate defaultReportType="tabular" versionFlags2="0"
   templateName="rwgreen"/>
  <reportWebSettings>
  <![CDATA[]]>
  </reportWebSettings>
</report>
</rw:objects>
-->

<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">

<HTML>

<HEAD>
  <TITLE> Your Title </TITLE>
  <rw:style id="rwgreen212">
<link rel="StyleSheet" type="text/css" href="css/rwgreen.css">
</rw:style>

  <META http-equiv="Content-Type" content="text/html; charset=ISO-8859-1">
</HEAD>

<BODY bgColor="#ffffff" link="#000000" vLink="#000000">

<TABLE border=0 cellPadding=0 cellSpacing=0 width="100%">
  <TBODY>
  <TR>
    <TD bgColor="#ffffff" rowSpan=2 vAlign=middle width=188><IMG alt="" src="images/rwgreen_logo.gif" width="135" height="36"></TD>
    <TD bgColor="#ffffff" height=40 vAlign=top><IMG alt="" height=1 src="images/stretch.gif" width=5></TD>
    <TD align=right bgColor="#ffffff" vAlign=bottom>&nbsp;</TD>
  </TR>
  </TBODY>
</TABLE>

<TABLE bgColor="#ff0000" border=0 cellPadding=0 cellSpacing=0 width="100%">
  <TBODY>
  <TR>
    <TD bgColor="#ff0000" class="OraColumnHeader">&nbsp;</TD>
  </TR>
  </TBODY>
</TABLE>

<TABLE border=0 cellPadding=0 cellSpacing=0 width="100%">
  <TBODY>
  <TR>
    <TD bgColor="#ff0000" vAlign=top class="OraColumnHeader"><IMG alt="" border=0 height=17 src="images/topcurl.gif" width=30></TD>
    <TD vAlign=top width="100%">
      <TABLE border=0 cellPadding=0 cellSpacing=0 width="100%">
        <TBODY>
        <TR>
          <TD bgColor="#000000" height=1><IMG alt="" border=0 height=1 src="images/stretch.gif" width=1></TD>
        </TR>
        <TR>
          <TD bgColor="#9a9c9a" height=1><IMG alt="" border=0 height=1 src="images/stretch.gif" width=1></TD>
        </TR>
        <TR>
          <TD bgColor="#b3b4b3" height=1><IMG alt="" border=0 height=1 src="images/stretch.gif" width=1></TD>
        </TR>
        </TBODY>
      </TABLE>
    </TD>
  </TR>
  </TBODY>
</TABLE>

<TABLE width="100%" border="0" cellspacing="0" cellpadding="0">
  <TR>
    <TD width="29%" valign="top">
      <TABLE width="77%" border="0" cellspacing="0" cellpadding="0">
        <TR>
          <TD width="10%" class="OraCellText"><IMG alt="" src="images/green_d_arrow.gif" width="10" height="10"></TD>
          <TD width="90%" class="OraCellText">Navigation goes here </TD>
        </TR>
        <TR>
          <TD width="10%" class="OraCellText"><IMG alt="" src="images/green_r_arrow.gif" width="10" height="10"></TD>
          <TD width="90%" class="OraCellText">Navigation Item</TD>
        </TR>
      </TABLE>
    </TD>
    <TD width="71%">
      <DIV align="center">
      <!-- Data Area Generated by Reports Developer -->
<rw:dataArea id="MGMENOGRPFR212">
<table class="OraTable">
<caption>  </caption>
 <!-- Header -->
 <thead>
  <tr>
   <th <rw:id id="HBMENO212" asArray="no"/> class="OraColumnHeader"> Meno </th>
   <th <rw:id id="HBPRIEZVISKO212" asArray="no"/> class="OraColumnHeader"> Priezvisko </th>
   <th <rw:id id="HBADRESA212" asArray="no"/> class="OraColumnHeader"> Adresa </th>
   <th <rw:id id="HBLOGIN212" asArray="no"/> class="OraColumnHeader"> Login </th>
   <th <rw:id id="HBTELEFON212" asArray="no"/> class="OraColumnHeader"> Telefon </th>
   <th <rw:id id="HBEMAIL212" asArray="no"/> class="OraColumnHeader"> Email </th>
   <th <rw:id id="HBDATUM212" asArray="no"/> class="OraColumnHeader"> Datum </th>
   <th <rw:id id="HBSTAV212" asArray="no"/> class="OraColumnHeader"> Stav </th>
   <th <rw:id id="HBNAZEV212" asArray="no"/> class="OraColumnHeader"> Nazev </th>
   <th <rw:id id="HBROKVYDANIA212" asArray="no"/> class="OraColumnHeader"> Rok Vydania </th>
   <th <rw:id id="HBISBN212" asArray="no"/> class="OraColumnHeader"> Isbn </th>
   <th <rw:id id="HBCENA212" asArray="no"/> class="OraColumnHeader"> Cena </th>
   <th <rw:id id="HBNAKLADATELSTVO212" asArray="no"/> class="OraColumnHeader"> Nakladatelstvo </th>
  </tr>
 </thead>
 <!-- Body -->
 <tbody>
  <rw:foreach id="RGMENO2121" src="G_MENO">
   <tr>
    <td <rw:headers id="HFMENO212" src="HBMENO212"/> class="OraCellText"><rw:field id="FMENO212" src="MENO" nullValue="&nbsp;"> F_MENO </rw:field></td>
    <td <rw:headers id="HFPRIEZVISKO212" src="HBPRIEZVISKO212"/> class="OraCellText"><rw:field id="FPRIEZVISKO212" src="PRIEZVISKO" nullValue="&nbsp;"> F_PRIEZVISKO </rw:field></td>
    <td <rw:headers id="HFADRESA212" src="HBADRESA212"/> class="OraCellText"><rw:field id="FADRESA212" src="ADRESA" nullValue="&nbsp;"> F_ADRESA </rw:field></td>
    <td <rw:headers id="HFLOGIN212" src="HBLOGIN212"/> class="OraCellText"><rw:field id="FLOGIN212" src="LOGIN" nullValue="&nbsp;"> F_LOGIN </rw:field></td>
    <td <rw:headers id="HFTELEFON212" src="HBTELEFON212"/> class="OraCellText"><rw:field id="FTELEFON212" src="TELEFON" nullValue="&nbsp;"> F_TELEFON </rw:field></td>
    <td <rw:headers id="HFEMAIL212" src="HBEMAIL212"/> class="OraCellText"><rw:field id="FEMAIL212" src="EMAIL" nullValue="&nbsp;"> F_EMAIL </rw:field></td>
    <td <rw:headers id="HFDATUM212" src="HBDATUM212"/> class="OraCellText"><rw:field id="FDATUM212" src="DATUM" nullValue="&nbsp;"> F_DATUM </rw:field></td>
    <td <rw:headers id="HFSTAV212" src="HBSTAV212"/> class="OraCellText"><rw:field id="FSTAV212" src="STAV" nullValue="&nbsp;"> F_STAV </rw:field></td>
    <td <rw:headers id="HFNAZEV212" src="HBNAZEV212"/> class="OraCellText"><rw:field id="FNAZEV212" src="NAZEV" nullValue="&nbsp;"> F_NAZEV </rw:field></td>
    <td <rw:headers id="HFROKVYDANIA212" src="HBROKVYDANIA212"/> class="OraCellNumber"><rw:field id="FROKVYDANIA212" src="ROK_VYDANIA" nullValue="&nbsp;"> F_ROK_VYDANIA </rw:field></td>
    <td <rw:headers id="HFISBN212" src="HBISBN212"/> class="OraCellText"><rw:field id="FISBN212" src="ISBN" nullValue="&nbsp;"> F_ISBN </rw:field></td>
    <td <rw:headers id="HFCENA212" src="HBCENA212"/> class="OraCellNumber"><rw:field id="FCENA212" src="CENA" nullValue="&nbsp;"> F_CENA </rw:field></td>
    <td <rw:headers id="HFNAKLADATELSTVO212" src="HBNAKLADATELSTVO212"/> class="OraCellText"><rw:field id="FNAKLADATELSTVO212" src="NAKLADATELSTVO" nullValue="&nbsp;"> F_NAKLADATELSTVO </rw:field></td>
   </tr>
  </rw:foreach>
 </tbody>
 <!-- Report Level Summary -->
 <tr>
  <th class="OraTotalText"> &nbsp; </th>
  <th class="OraTotalText"> &nbsp; </th>
  <th class="OraTotalText"> &nbsp; </th>
  <th class="OraTotalText"> &nbsp; </th>
  <th class="OraTotalText"> &nbsp; </th>
  <th class="OraTotalText"> &nbsp; </th>
  <th class="OraTotalText"> &nbsp; </th>
  <th class="OraTotalText"> &nbsp; </th>
  <th class="OraTotalText"> &nbsp; </th>
  <th class="OraTotalText"> &nbsp; </th>
  <th class="OraTotalText"> &nbsp; </th>
  <td <rw:headers id="HFSumCENAPerReport212" src="HBCENA212"/> class="OraTotalNumber">Total: <rw:field id="FSumCENAPerReport212" src="SumCENAPerReport" nullValue="&nbsp;"> F_SumCENAPerReport </rw:field></td>
  <th class="OraTotalText"> &nbsp; </th>
 </tr>
</table>
</rw:dataArea> <!-- id="MGMENOGRPFR212" -->
<!-- End of Data Area Generated by Reports Developer -->

      </DIV>
    </TD>
  </TR>
</TABLE>

<P>
  <BR>
</P>

<TABLE border=0 cellPadding=0 cellSpacing=0 width="100%">
  <TBODY>
  <TR>
    <TD bgColor="#000000" colSpan=2><IMG alt="" border=0 height=1 src="images/stretch.gif" width=1></TD>
  </TR>
  <TR>
    <TD bgColor="#ff0000" colSpan=2 class="OraColumnHeader"><IMG alt="" border=0 height=4 src="images/stretch.gif" width=1></TD>
  </TR>
  <TR>
    <TD bgColor="#000000" colSpan=2><IMG alt="" border=0 height=1 src="images/stretch.gif" width=1></TD>
  </TR>
  <TR>
    <TD bgColor="#ffffff">&nbsp;</TD>
    <TD align=right bgColor="#ffffff">&nbsp;</TD>
  </TR>
  </TBODY>
</TABLE>

</BODY>

</HTML>

<!--
</rw:report>
-->
