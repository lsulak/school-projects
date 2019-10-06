<div>
    <form action="_heslo.php" method="post">
        <label for="inputtext2">E-mail:</label>
        <input maxlength="50" id="inputtext2" type="text" name="email" value='<? echo $_GET["email"]; ?>'/>
        <input type="hidden" name="page" value='<? echo $redir; ?>'/>
        Na zadaný email Vám přijde heslo<br><br>
        <input type='submit' value="Odoslat heslo"/>
    </form>
</div>