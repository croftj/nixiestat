create table sensor_types
(
   type     varchar(32),
   ident    serial primary key
);

create table sensors
(
   sensor_id   varchar(256),
   type        varchar(32),
   ident       serial primary key
);

create table devices
(
   name           varchar(32),
   type           varchar(32),
   conf_topic     varchar(32),
   time_topic     varchar(32),
   log_topic      varchar(32),
   temp_topic     varchar(32),
   "timestamp"    int,
   ident          serial primary key
);

create table device_sensor
(
   sensor_fk   int references sensors,
   device_fk   int references devices,
   ident       serial primary key
);

create table temperatures
(
   device      varchar(32),
   sensor      varchar(256),
   temperature numeric(7,3),
   average     numeric(7,3),
   minimum     numeric(7,3),
   maximum     numeric(7,3),
   "timestamp" int,
   ident       serial primary key
);

