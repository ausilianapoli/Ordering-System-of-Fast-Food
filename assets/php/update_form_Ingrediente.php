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
	<h2>Modifica un ingrediente</h2>
	<p>Inserisci uno degli ingredienti presenti nel sistema: </p>
	<?php
		$sql = "SELECT * FROM ingrediente;";
		$result = mysqli_query($conn, $sql);
		$resultCheck = mysqli_num_rows($result);

		if ($resultCheck > 0) {
			while($row = mysqli_fetch_assoc($result)){
				echo "(".$row['ID_Ingrediente'].", ".$row['Nome'].", ".$row['Note'].")<br>";
			}
		}
	?>
	<p></p>
	<form action="update_Ingrediente.php" method="POST">
		<input type="text" name="ID_Ingrediente_old" placeholder="ID ingrediente vecchio - Es. I01">
		<input type="text" name="ID_Ingrediente_new" placeholder="ID ingrediente nuovo - Es. I02">
		<input type="text" name="nome_new" placeholder="Nome nuovo - Es. uovo bio">
		<input type="text" name="note" placeholder="Note - Es. prodotto di stagione">
		<button type="submit" name="submit">Modifica</button>
	</form>
	<p></p>
	<div class="vertical-menu">
		<a href="../../index.php" class=active>Torna alla Home</a>
	</div>
</body>
</html>