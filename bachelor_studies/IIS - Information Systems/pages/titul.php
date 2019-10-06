<div>
    <?
    $t_id = $_GET["id"];
    $titul = mysqli_query($db, "SELECT * FROM titul WHERE id='$t_id'");
    $row = mysqli_fetch_assoc($titul);
    $autor_titul = mysqli_query($db, "SELECT id_autor FROM autor_titul WHERE id_titul='$t_id'");
    ?>
    <h2><font color="black"> Informace o titulu: </font><? echo $row["nazev"]; ?></h2>
    <br><br>

    <div class="caption">Titul : <? echo $row["nazev"]; ?></div>
    <div id="table">
        <div class="header-row row">
            <span class="cell primary">Název</span>
            <span class="cell">Autor</span>
            <span class="cell">Rok vydání</span>
            <span class="cell">ISBN</span>
            <span class="cell">Nakladatelsvé</span>
            <span class="cell">Dostupnost</span>
        </div>
        <div class="row">
				<span class="cell primary" data-label="jmeno"><?
                    echo $row["nazev"]; ?></span>
            <tr><span class="cell" data-label="autor">
					<? while ($row_at = mysqli_fetch_array($autor_titul)) {
                        $a_id = $row_at["id_autor"];
                        $autor = mysqli_query($db, "SELECT * FROM autor WHERE id='$a_id'");
                        $row_a = mysqli_fetch_assoc($autor); ?>
                    <a title="Zobrazit knihy autora" href='index.php?pg=tituly&id=<? echo $row_a["id"]; ?>'>
                        <? echo $row_a["prijmeni"] . ", " . $row_a["jmeno"] ?></a><?
                    }?>
					</span>
                <span class='cell' data-label='rok_vydani'> <? echo $row["rok_vydani"] ?> </span>
                <span class='cell' data-label='isbn'> <? echo $row["isbn"] ?></span>
                <span class='cell' data-label='nakladatelstvi'> <? echo $row["nakladatelstvi"] ?></span>
            </tr>
            <span class='cell' data-label='dostupnost'><? include 'pages/inc_rez.php'; ?></span>
        </div>
    </div>
</div>