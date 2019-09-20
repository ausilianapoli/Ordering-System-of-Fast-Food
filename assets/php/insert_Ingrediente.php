<?php

	include_once 'dbh.php';

	if(isset($_POST['submit'])){
		$ingrediente = mysqli_real_escape_string($conn, $_POST['ID_Ingrediente']);
		$nome = mysqli_real_escape_string($conn, $_POST['nome']);
		$note = floatval(mysqli_real_escape_string($conn, $_POST['note']));

		$sql = "INSERT INTO ingrediente VALUES ('$ingrediente','$nome','$note');";
		mysqli_query($conn, $sql);

		header("Location: ../php/success.php");
	}
	else{
		header("Location: ../php/error.php");
	}

?>