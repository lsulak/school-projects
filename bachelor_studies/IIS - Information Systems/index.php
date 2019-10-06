<?php

session_start(); // start session

if($_SESSION['logged']==0)
{
	$_SESSION["status"] = 0;
	//0 == neprihlasen
	//1 == prihlasen
	$_SESSION["role"] = 0;
	//0 == neprihlasen
	//1 == uzivatel
	//2 == zamestnanec
	//3 == administrator 
} 
else 
{
	$_SESSION["status"] = 1;
	if (isset($_SESSION['timeout']) && $_SESSION['status']!=0) 
	{
    	$session_life = time() - $_SESSION['timeout'];
    	if (($session_life > 900 ) && $_SESSION['status']!=0) //Po 15 minutach automaticky odhlasi, ak je prihlaseny
    	{
        	session_destroy();
        	header("Location: _logout.php");
    	}
	}
	$_SESSION['timeout'] = time();
}

if (!isset($_GET['pg'])) //pokud v adrese neni &pg, nastavi se defaultne hlavni stranka
	$pg = "main";
else
	$pg = $_GET['pg']; 

include "mysql.php";

?>
<!DOCTYPE html>
<html>
	<head>
		<title>Knihovna ITI</title>
	    <meta http-equiv="content-type" content="text/html; charset=UTF-8">
	    <meta name="description" content="Projekt do predmetu IIS">
	    <meta name="author" content="Ladislav Šulák, Petr Švaňa">
	    <link href="style.css" rel="stylesheet" type="text/css">
	    <link rel="icon" href="images/book.ico">
	    <script type="text/javascript" src="javascript/basiccalendar.js"></script>
	    <script type="text/javascript" src="javascript/ucet.js"></script>
	    <script type="text/javascript" src="javascript/googleMap.js"></script>
		<script src="https://maps.googleapis.com/maps/api/js?sensor=false" type="text/javascript"></script>
   		<script type="text/javascript">
    		google.maps.event.addDomListener(window, 'load', initialize);
   		</script>
	</head>
	<body onload="SidePanelHide1(); SidePanelHide2(); SidePanelHide3();">
		<!--Header: Logo, Topmenu, Menu -->
		<div id="header">
		    <div id="logo">
		      <h1><a href="index.php">Knihovna ITI</a></h1>
		      <h2><a href="index.php">Information technology Institute</a></h2>
		    </div>
		    <div id="topmenu">
		      <ul>
		        <li><a href="index.php" id="topmenu1" >Domů</a></li>
		        <li><a href="index.php?pg=kontakt" id="topmenu3" >Kontakt</a></li>
		        <li><a href="index.php?pg=mapa_stranek" id="topmenu2" >Mapa stránek</a></li>
		        <li><a href="index.php?pg=ucet" id="topmenu4">Můj účet</a></li>
		      </ul>
		    </div>
		</div>

		<div id="menu">
		    <ul>
		      <li class="first"></li>
		      <a href="index.php?pg=aktuality">Aktuálně</a>
		      <li><a href="index.php?pg=o_knihovne">O knihovně</a></li>
		      <li><a href="index.php?pg=sluzby">Služby</a></li>
		      <li><a href="index.php?pg=tituly">Tituly</a></li>
		      <li><a href="index.php?pg=zamestnanci">Zaměstnanci</a></li>
		      <? if($_SESSION['role']>=2) { ?>
		      	 <li><a href="index.php?pg=ctenari">Čtenáři</a></li>
		      <?; } ?>
		    </ul>
		</div>
		
		<!-- Obsah stranky -->
		<div id="content">
   			<div id="main">
				<?php
				if(isset($_GET["msg"])) //include zprav
					include 'pages/msg.php';
				if(isset($_GET["pg"])) // promenna pro navrat na spravnou stranku
					$redir = $_GET["pg"];
				else $redir = "main";	
				if($pg == "aktuality" or $pg == "kontakt" or //include obsahu
				  $pg == "mapa_stranek" or $pg == "o_knihovne" or
				  $pg == "sluzby")
					include 'pages/' . $pg . '.html';
				else
					include 'pages/' . $pg . '.php';
				?>
			</div>
		<!--SIDEBARS!!!!!!!!!!!-->
	    <div id="sidebar">
            <div id="zam_menu" class="boxed">
                <h2 id="hledej" class="title">[Reset databáze]</h2>
                <p>Provede restartování databáze do původního stavu</p>
                <? if($_SESSION['status']!=1) { ?>
                <a href="javascript:SidePanelShow(document.getElementById('hidden3'));">Opravdu chci restartovat DB</a>
                <div id="hidden3">
                    <br /><? } ?>
                    <form method="post" action="_resetdb.php">
                        <input type="hidden" name="page" value='<? echo $redir; ?>'/>
                        <input type="submit" value="Restartovat DB"/>
                    </form>
                <? if($_SESSION['status']!=1) { ?></div><? } ?>
            </div>

	    	<? if($_SESSION['role']==3) { ?>
	    	<div id="zam_menu" class="boxed">
	        	<h2 id="hledej" class="title">Administrace</h2>	  
	        	<a href="index.php?pg=add&co=zamestnanec">Přidat zaměstnance</a><br>
	        	<a href="index.php?pg=rm&co=zamestnanec">Zrušit zaměstnance</a><br>
	        </div>
	        <? } ?>
	        <? if($_SESSION['role']>=2) { ?>
	    	<div id="zam_menu" class="boxed">
	        	<h2 id="hledej" class="title">Zaměstnanec</h2>
	        	<a href="index.php?pg=man&co=rezervace">Spravovat rezervace</a><br>
	        	<br>
	        	<a href="index.php?pg=man&co=vypujcka">Spravovat aktuální výpůjčky</a><br>
	        	<a href="index.php?pg=add&co=vypujcka">Vložit výpůjčku</a><br>
	        	<br>
	        	<a href="index.php?pg=add&co=vytisk">Přidat výtisk(y) titulu</a><br>
	        	<a href="index.php?pg=rm&co=vytisk">Vyřadit výtisk</a><br>
	        	<br>
	        	<a href="index.php?pg=add&co=autor">Přidat autora</a><br>
	        	<a href="index.php?pg=edit&co=autor">Editovat autora</a><br>
	        	<a href="index.php?pg=rm&co=autor">Zrušit autora</a><br>
	        	<br>
	        	<a href="index.php?pg=add&co=titul">Přidat titul</a><br>
	        	<a href="index.php?pg=edit&co=titul">Editovat titul</a><br>
	        	<a href="index.php?pg=rm&co=titul">Zrušit titul</a><br>
	        </div>
	        <? } ?>


	      	<div id="login" class="boxed">
		        <h2 class="title">Vaše konto</h2>
		        <div class="content">
		            <div class='container'>
	                    <?
	                    $lg = $_SESSION["logged"];
	                    if($_SESSION["role"]==1) {	                    	
	                    	$uzi = mysqli_query($db, "SELECT jmeno, prijmeni FROM ctenar WHERE id='$lg'");
	                    	while ($row=mysqli_fetch_array($uzi)) {
	                    		echo "Uživatel: ".$row["jmeno"]." ".$row["prijmeni"];
	                    	}	                    	
	                    } elseif($_SESSION["role"]>1) {
							$uzi = mysqli_query($db, "SELECT jmeno, prijmeni FROM zamestnanec WHERE id='$lg'");
	                    	while ($row=mysqli_fetch_array($uzi)) {
	                    		if($_SESSION["role"]==2)
	                    			echo "Zaměstnanec: ".$row["jmeno"]." ".$row["prijmeni"];
	                    		else
	                    			echo "Administrátor: ".$row["jmeno"]." ".$row["prijmeni"];
	                    	}
	                    }
                    	include 'pages/login.php'; ?>
	                    
	                    <? if($_SESSION["status"]==0) { ?>
	                   
	                    <p><a href="javascript:SidePanelShow(document.getElementById('hidden'));
                  		 javascript:SidePanelHide2();SidePanelHide3();">Registrace</a></p>
	                    <div id="hidden">
	                    <?php 
	                    	include 'pages/registrace.php'; ?>
	                    </div>
						<br>

						<div id="send_pass"><a href="javascript:SidePanelShow(document.getElementById('hidden2'));
                  		javascript:SidePanelHide1();SidePanelHide3();">Zapomněli jste své heslo?</a>
                  		</div>

		                <br>
		                <div id="hidden2">
		                	<?php 
	                    	include 'pages/odeslat_heslo.php'; ?>
		                </div>
		           		<? ;}  ?>
	                </div>
	        	</div>
	      	</div>
            <div id="updates2" class="boxed">
                <h2 id="hledej" class="title">Vyhledáváni</h2>
                <p>autorů, zaměstnanců a titulů</p>
                <form action="index.php?pg=hledej"  method="post">
                    <input id = "vyhl_obr" type="image" src="images/Search.png" alt="Submit" width="15" height="15" />
                    <input id = "vyhl_obr2" type="image" src="images/Search.png" alt="Submit" width="15" height="15" />
                    <input maxlength="100" id = "vyhledej" type="text" name="find" onblur="sBlur(this)"  onfocus="sFocus(this)" value="Hledat výraz"/></br>
                </form>
            </div>
	    <div id="updates" class="boxed">
	        <h2 class="title">Nově přidané tituly</h2>
	        <div class="content">
	          <? include 'pages/nove_tituly.php'; ?>
	        </div>
	    </div>
	    <div id="partners" class="boxed">
	        <h2 class="title">Kalendář</h2>

	          <form id="kalendar">
	            <div id="calendarspace">
	              <script>
	                document.write(buildCal(curmonth,curyear,"main", "month", "daysofweek", "days", 1))
	              </script>
	            </div>
	            <select onChange="updatecalendar(this.options)">
	              <script type="text/javascript">
	                document.write('<option value="'+(curmonth-1)+'" selected="yes">Zvolte měsíc</option>')
	                for (i=0; i<12; i++) 
	                  document.write('<option value="'+i+'">'+themonths[i]+' '+curyear+'</option>');
	              </script>
	            </select>
	          </form>

	          </div>
	        </div>

	      </div>
	    </div>

		<!--FOOT!!!!-->
	    <div id="footer">
	      <p id="legal"> 
	        <span id="left">Projekt do předmětu IIS - Knihovna</span> 
	        <span id="right"> &copy; 2014 Ladislav Šulák, Petr Švaňa</span></p>
	      <p id="links"><a href="index.php">Domů</a> | <a href="index.php?pg=mapa_stranek">Mapa stránek</a></p>
	    </div>

	</body>
</html>