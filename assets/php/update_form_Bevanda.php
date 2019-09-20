<?php
	include_once 'dbh.php';
?>

<!DOCTYPE html>
<html lang=ita>
<head><meta charset="utf-8">
	<meta name="viewport" content="width=device-width, initial-scale=1">
	<link rel="stylesheet" type="text/css" href="../css/style.css">
	<title>La mia Panineria</title>
</head>
<body>
	<h2>Modifica una bevanda</h2>
	<p>Inserisci una delle bevande presenti nel sistema: </p>
	<?php
		$sql = "SELECT * FROM bevanda;";
		$result = mysqli_query($conn, $sql);
		$resultCheck = mysqli_num_rows($result);

		if ($resultCheck > 0) {
			while($row = mysqli_fetch_assoc($result)){
				echo "(".$row['ID_Bevanda'].", ".$row['Nome'].", ".$row['prezzo_EUR'].")<br>";
			}
		}
	?>
	<p></p>
	<form action="update_Bevanda.php" method="POST">
		<input type="text" name="ID_Bevanda_old" placeholder="ID bevanda vecchia - Es. B01">
		<input type="text" name="ID_Bevanda_new" placeholder="ID bevanda nuova - Es. B02">
		<input type="text" name="nome_new" placeholder="Nome nuovo - Es. The San Benedetto">
		<input type="text" name="prezzo_new" placeholder="Prezzo nuovo (EUR) - Es. 1.20">
		<button type="submit" name="submit">Modifica</button>
	</form>
	<p></p>
	<div class="vertical-menu">
		<a href="../../index.php" class=active>Torna alla Home</a>
	</div>
</body>
</html>