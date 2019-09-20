<!DOCTYPE html>
<html lang=ita>
<head><meta charset="utf-8">
	<meta name="viewport" content="width=device-width, initial-scale=1">
	<link rel="stylesheet" type="text/css" href="../css/style.css">
	<title>La mia Panineria</title>
</head>
<body>
	<h2>Inserisci un nuovo ingrediente</h2>
	<form action="insert_Ingrediente.php" method="POST">
		<input type="text" name="ID_Ingrediente" placeholder="ID ingrediente - Es. I01">
		<input type="text" name="nome" placeholder="Nome - Es. Hot Dog">
		<input type="text" name="note" placeholder="Note - Es. prodotto surgelato">
		<button type="submit" name="submit">Inserisci</button>
	</form>
	<p></p>
	<div class="vertical-menu">
		<a href="../../index.php" class=active>Torna alla Home</a>
	</div>
</body>
</html>