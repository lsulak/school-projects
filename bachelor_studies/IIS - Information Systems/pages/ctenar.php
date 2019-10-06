<?php
if ($_SESSION['role'] < 2)
    echo "Stranka dostupná pouze zaměstnancům.";
else {
    ?>

    <div>
        <?
        $c_id = $_GET["id"];
        $query_str = "SELECT * FROM ctenar WHERE id='$c_id'";
        $ctenar = mysqli_query($db, $query_str);
        while ($row = mysqli_fetch_array($ctenar)) {
            ?>
            <h2><span style="color:black">Informace o čtenáři:<? echo $row["jmeno"] . " " . $row["prijmeni"] ?></span>
            </h2>

            <br><br>
            <div class="caption">Čtenař : <? echo $row["jmeno"] . " " . $row["prijmeni"] ?></div>
            <div id="table">
                <div class="header-row row">
                    <span class="cell primary">Jméno</span>
                    <span class="cell primary">Login</span>
                    <span class="cell primary">Adresa</span>
                    <span class="cell primary">Telefon</span>
                    <span class="cell primary">Email</span>
                    <span class="cell primary">Rok registrace</span>
                </div>

                <div class="row">
				<span class="cell primary" data-label="jmeno"><?
                    $c_id = $row["id"];
                    ?><a href='index.php?pg=ctenar&id=<? echo $c_id; ?>'>
                        <? echo $row["prijmeni"] . ", " . $row["jmeno"] . "</a>"; ?>
				</span>

				<span class="cell" data-label="login"><?
                    echo $row["login"]; ?>
				</span>

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
<? } ?>