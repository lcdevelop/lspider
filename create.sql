create database lspider;
use lspider;
create table link (
    `sign` VARCHAR(24) NOT NULL,
    `url` VARCHAR(1024) NOT NULL DEFAULT '',
    `maindomain` VARCHAR(80) DEFAULT '',
    `ip` VARCHAR(16) DEFAULT '',
    `prelink` VARCHAR(1024) DEFAULT '',
    `preanchor` VARCHAR(1024) DEFAULT '',
    `weight` INT(10) unsigned NOT NULL DEFAULT 1,
    `linkdepth` INT(10) unsigned NOT NULL DEFAULT 0,
    `crawlstate` INT(10) NOT NULL DEFAULT 0,
    `crawlretry` INT(10) NOT NULL DEFAULT 0,
    `hub` BOOL NOT NULL DEFAULT FALSE,
    `fresh` BOOL NOT NULL DEFAULT FALSE,
    `updatetime` TIMESTAMP,
    `foundtime` TIMESTAMP,
    `crawledtime` TIMESTAMP,
    PRIMARY KEY (`sign`)
) ENGINE=MyISAM DEFAULT character set utf8;
