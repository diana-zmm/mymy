# Dokumentation Diana Zimmermann - Würfelspiel

## Grundlegende Projektfunktionalität:
* Funktioniert im Allgemeinen wie ein Standard-Schachspiel, Regeln wie En Passant und Rochade wurden ebenfalls implementiert.
* Alle paar Runden wird für den Spieler, der am Zug ist, eine Zahl gewürfelt, die für eine bestimmte Auslösehandlung sorgt
* Die Zahlen mit ihren Auslösehandlungen sind wie folgt definiert:
    1. Es wird eine Figur des Spielers, der am Zug ist, ermittelt, mit der als nächstes gespielt werden muss.
    2. Es wird eine Richtung ermittelt, in die sich als nächstes bewegt werden muss.
    3. Anstatt seinen eigenen Zug auszuführen, ist der Gegner wieder am Zug.
    4. Der Spieler, der an der Reihe ist, darf eine Figur des Gegners bestimmen, mit der der Gegner in der nächsten Runde nicht ziehen darf.
    5. Der Spieler, der an der Reihe ist, darf ein Feld auf dem Brett bestimmen, auf das der Gegner in der nächsten Runde nicht ziehen darf.
    6. Für diese Runde dürfen Bauern nur wie Springer bewegt werden.
* Zur Sicherheit wurde ein Notfallcode eingeführt, falls bestimmte Auslösehandlungen das weitere Spielen unmöglich machen. Wird ein 'x' eingegeben, werden alle Auslösehandlungen rückgängig gemacht. Dies kann beispielsweise der Fall sein, wenn ein Spieler nur noch auf ein bestimmtes Feld ziehen kann, dieses jedoch eingefroren ist (5.). 


# Die wichtigsten Klassen und Funktionen
* Klasse Piece + vererbte Figuren Pawn, Rook, Knight, Bishop, Queen, King: besitzen jeweils eine CanMove-Funktion, die überprüft, ob ein Zug mit der jeweiligen Figur valide ist
* Struct Move: beinhaltet das Anfangs- und Zielfeld, die beteiligten Figuren und weitere relevante Informationen
* Schachmatterkennung: 
    1. isKinginCheck: prüft, ob der König im aktuellen Zustand des Bretts im Schach steht
    2. canMove_notCheck: führt einen Zug aus und prüft, ob der König danach im Schach steht
    3. countCheckThreats: zählt die Anzahl an Angreifern des Königs und speichert deren Position 
    4. canKingEscape: prüft, ob der König auf ein Feld ziehen kann, auf dem er nicht im Schach steht
    5. canCaptureAttacker: prüft, ob eine Figur (Angreifer) geschlagen werden kann
    6. canBlockCheck: prüft, ob das Schach eines Angreifers von einer Figur geblockt werden kann
    7. isCheckmate: bündelt alle Funktionen zur Schachmatterkennung und prüft, ob ein Schachmatt vorliegt
* Würfelspiel: 
    1. handleDiceRoll: setzt Hilfsfunktion in Gang, die gewürfelt wurde
    2. movePiece: ermittelt eine Figur, die als nächstes gespielt werden muss und die auch bewegt werden kann
    3. moveDirection: ermittelt eine Richtung, in die gezogen werden muss und in die auch gezogen werden kann
    4. switchTurn: wechselt die Reihenfolge, sodass ein Zug übersprungen wird
    5. chooseFrozenPiece: aktiviert chooseFrozen, was im nächsten Durchlauf der while-Schleife die eingefrorene Figur verarbeitet
    6. chooseSquareForOpponent: aktiviert chooseSquare, was im nächsten Durchlauf der while-Schleife das eingefrorene Feld verarbeitet
    7. activatePawnsLikeKnights: aktiviert PawnsLikeKnights, was im nächsten Durchlauf alle Bauern wie Springer ziehen lässt
    8. undoChanges: macht alle Besonderheiten des Würfelspiels rückgängig

# Programmierkonzepte
* Struktur 'Move'
* Vererbung: abstrakte Klasse Piece -> Subklassen der Figuren
* Getrennte Kompilierung und Modularität mit Header-Dateien
* verschiedene Container, um Elemente abzulegen: 
    1. zweidimensionales Feld 'board' zur Darstellung und Veränderung des Schachbretts
    2. Vector-Paare für 'attackers' und 'WhitePositions' und 'BlackPositions'
    3. String für den Input
    4. Map zum Figuren-Mapping
* Zufallszahlen mit 'rand'

# Link zum Video in der Cloud
https://cloud.uni-konstanz.de/index.php/s/TNLaGokJawgqbAG 
