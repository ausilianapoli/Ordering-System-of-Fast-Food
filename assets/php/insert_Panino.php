<?php

	include_once 'dbh.php';

	if(isset($_POST['submit'])){
		$panino = mysqli_real_escape_string($conn, $_POST['ID_Panino']);
		$nome = mysqli_real_escape_string($conn, $_POST['nome']);
		$prezzo = floatval(mysqli_real_escape_string($conn, $_POST['prezzo']));

		$sql = "INSERT INTO panino VALUES ('$panino','$nome','$prezzo');";
		mysqli_query($conn, $sql);

		header("Location: ../php/success.php");
	}
	else{
		header("Location: ../php/error.php");
	}

?>