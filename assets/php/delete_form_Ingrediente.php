<!DOCTYPE html>
<html lang=ita>
<head><meta charset="utf-8">
	<meta name="viewport" content="width=device-width, initial-scale=1">
	<link rel="stylesheet" type="text/css" href="../css/style.css">
	<title>La mia Panineria</title>
</head>
<body>
	<h2>Elimina un ingrediente</h2>
	<form action="delete_Ingrediente.php" method="POST">
		<input type="text" name="ID_Ingrediente" placeholder="ID ingrediente - Es. I01">
		<button type="submit" name="submit">Elimina</button>
	</form>
	<p></p>
	<div class="vertical-menu">
		<a href="../../index.php" class=active>Torna alla Home</a>
	</div>
</body>
</html>