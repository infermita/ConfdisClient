PRAGMA foreign_keys=OFF;
BEGIN TRANSACTION;
CREATE TABLE `images` (
	`id`	char(8),
	`cardkey`    char(8),
	`image`	TEXT,
	PRIMARY KEY(id,cardkey)
);
COMMIT;
