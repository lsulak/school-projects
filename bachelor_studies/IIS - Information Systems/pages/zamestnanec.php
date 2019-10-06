<div>
    <?
    $z_id = $_GET["id"];
    $query_str = "SELECT * FROM zamestnanec WHERE id='$z_id'";
    $zamestnanec = mysqli_query($db, $query_str);
    while ($row = mysqli_fetch_array($zamestnanec)) {
        ;?>
        <h2><font color="black"> Informace o zaměstnanci: </font>
            <? echo $row["jmeno"] . " " . $row["prijmeni"] ?></h2>
        <br><br>
        <div class="caption">Zaměstnanec : <? echo $row["jmeno"] . " " . $row["prijmeni"] ?></div>
        <div id="table">
            <div class="header-row row">
                <span class="cell primary">Jméno</span>
                <span class="cell primary">Adresa</span>
                <span class="cell primary">Telefon</span>
                <span class="cell primary">Email</span>
                <span class="cell primary">Rok nástupu</span>
            </div>
            <div class="row">
				<span class="cell primary" data-label="jmeno"><?
                    $z_id = $row["id"];
                    ?><a
                        href='index.php?pg=zamestnanec&id=<? echo $z_id; ?>'><? echo $row["prijmeni"] . ", " . $row["jmeno"] . "</a>"; ?></span>
				<span class="cell" data-label="adresa"><?
                    echo $row["adresa"]; ?></span>
				<span class="cell" data-label="telefon"><?
                    echo $row["telefon"]; ?></span>
				<span class="cell" data-label="email">
					<a href="mailto:<? echo $row["email"]; ?>"><?
                        echo $row["email"]; ?></a>
				</span>
				<span class="cell" data-label="rok_nastupu"><?
                    $nastup = $row["cas_vytvoreni"];
                    echo substr($nastup, 0, 4) ?></span>
            </div>
        </div>
    <? } ?>
</div>