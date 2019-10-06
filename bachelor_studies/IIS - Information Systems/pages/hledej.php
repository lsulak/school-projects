<?php
$find = $_POST['find'];
if (strlen($find) < 3) {
    ?>
    <div id="msg2">Vyhledávání není dovoleno pro řetězce kratší než 3 znaky.</div>
<?
} else {
    ?>
    <h2>Hledaný výraz: <? echo $find; ?></h2>
    <?
    $titul = mysqli_query($db, "SELECT * FROM titul WHERE nazev LIKE '%$find%'");
    $autor = mysqli_query($db, "SELECT * FROM autor WHERE prijmeni LIKE '%$find%' OR jmeno LIKE '%$find%'");
    $zamestnanci = mysqli_query($db, "SELECT * FROM zamestnanec WHERE prijmeni LIKE '%$find%'");
    $t_cnt = mysqli_num_rows($titul);
    $a_cnt = mysqli_num_rows($autor);
    $z_cnt = mysqli_num_rows($zamestnanci);
    if ($t_cnt == 0)
        echo "<p>Nebyl nalezen žádný titul odpovídající tomuto řetězci.</p><br>";
    else {
        ?>
        <div class="caption">Tituly odpovídající výrazu "<? echo $find ?>"</div>
        <div id="table">
            <div class="header-row row">
                <span class="cell primary">Název</span>
                <span class="cell">Autor</span>
                <span class="cell">Rok vydání</span>
                <span class="cell">ISBN</span>
                <span class="cell">Nakladatelsví</span>
                <span class="cell">Dostupnost</span>
            </div>
            <?
            while ($row = mysqli_fetch_array($titul)) {
                $t_id = $row["id"];
                $autor_titul = mysqli_query($db, "SELECT id_autor FROM autor_titul WHERE id_titul='$t_id'");
                ?>
                <div class="row">
					<span class="cell primary" data-label="nazev"><?
                        echo "<a href='index.php?pg=titul&id=" . $row["id"] . "'>" . $row["nazev"] . "</a>"; ?></span>
                    <tr><span class="cell" data-label="autor">
						<? while ($row_at = mysqli_fetch_array($autor_titul)) {
                            $a_id = $row_at["id_autor"];
                            $autor = mysqli_query($db, "SELECT * FROM autor WHERE id='$a_id'");
                            while ($row_a = mysqli_fetch_array($autor)) {
                                ?>
                            <a title="Zobrazit knihy autora" href='index.php?pg=tituly&id=<? echo $row_a["id"]; ?>'>
                                <? echo $row_a["prijmeni"] . ", " . $row_a["jmeno"] ?></a><?
                            }
                        }?>
						</span>
                        <span class='cell' data-label='rok_vydani'> <? echo $row["rok_vydani"] ?> </span>
                        <span class='cell' data-label='isbn'> <? echo $row["isbn"] ?></span>
                        <span class='cell' data-label='nakladatelstvi'> <? echo $row["nakladatelstvi"] ?></span>
                    </tr>
                    <span class='cell' data-label='dostupnost'><? include 'pages/inc_rez.php'; ?></span>
                </div>
                <? ;
            } ?>
        </div><br><br>
    <?
    }
    if ($a_cnt == 0)
        echo "<p>Nebyl nalezen žádný autor odpovídající tomuto řetězci.</p><br>";
    else {
        ?>
        <div class="caption">Autoři odpovídající výrazu "<? echo $find; ?>"</div>
        <div id="table">
            <div class="header-row row">
                <span class="cell primary">Jméno</span>
                <span class="cell">Počet knih</span>
                <span class="cell">Rok narození</span>
            </div>
            <?    $autor = mysqli_query($db, "SELECT * FROM autor WHERE prijmeni LIKE '%$find%' OR jmeno LIKE '%$find%'");
            while ($row = mysqli_fetch_array($autor)) {
                //var_dump($row);
                ?>

                <div class="row">
					<span class="cell primary" data-label="jmeno"><?
                        echo "<a href='index.php?pg=tituly&id=" . $row['id'] . "'>" . $row["prijmeni"] . ", " . $row["jmeno"] . "</a>"; ?></span>
                    <tr><span class="cell" data-label="pocet_knih"><?
                            $a_id = $row['id'];
                            echo mysqli_num_rows(mysqli_query($db, "SELECT * FROM autor_titul WHERE id_autor='$a_id'")); ?></span>
                    </tr>
                    <span class="cell" data-label="rok_narozeni"><? echo substr($row["datum_narozeni"], -4); ?></span>
                </div>

            <? } ?>
        </div><br><br>
    <?
    }
    if ($z_cnt == 0)
        echo "<p>Nebyl nalezen žádný zaměstnanec odpovídající tomuto řetězci.</p><br>";
    else {
        ?>
        <div class="caption">Zaměstnanci odpovídající výrazu "<? echo $find; ?>"</div>
        <div id="table">
            <div class="header-row row">
                <span class="cell primary">Jméno</span>
                <span class="cell primary">Adresa</span>
                <span class="cell primary">Telefon</span>
                <span class="cell primary">Email</span>
                <span class="cell primary">Rok nástupu</span>

            </div>
            <?    $zamestnanci = mysqli_query($db, "SELECT * FROM zamestnanec WHERE prijmeni LIKE '%$find%'");
            while ($row = mysqli_fetch_array($zamestnanci)) {
                ;?>
                <div class="row">
					<span class="cell primary" data-label="jmeno"><?
                        $z_id = $row["id"];
                        ?><a href='index.php?pg=zamestnanec&id=<? echo $z_id; ?>'>
                        <? echo $row["prijmeni"] . ", " . $row["jmeno"] . "</a>"; ?></span>
					<span class="cell" data-label="adresa"><?
                        echo $row["adresa"]; ?></span>

						<span class="cell" data-label="telefon"><?
                            echo $row["telefon"]; ?></span>

						<span class="cell" data-label="email"><?
                            echo $row["email"]; ?></span>

						<span class="cell" data-label="rok_nastupu"><?
                            $nastup = $row["cas_vytvoreni"];
                            echo substr($nastup, 0, 4) ?></span>
                </div>

            <? } ?>
        </div>
    <? }
} ?>
<h2>Vyhledávání</h2>
<form action="index.php?pg=hledej" method="post" id="reg">
    <br><br><br><br>
    <input maxlength="100" id="inputtext2" style="width:330px;height:30px;" type="text" name="find"
           placeholder="Vyhledavani autoru, titulu a zamestnancu"/><br>
    <input type="submit" value="hledej"/>
</form>

