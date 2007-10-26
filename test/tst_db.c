
main () 
{
  Database *DB;
  Datablock *db;

  DB = DBcreate("test");

  db = dbcreate (DB, "integers", 'i', 1000);
  db = dbcreate (DB, "reals", 'r', 1000);
  db = dbcreate (DB, "coordinates", 'x', 1000);
  db = dbcreate (DB, "hkls", 'h', 1000);
  db = dbcreate (DB, "text", 't', 1000);
  db = dbcreate (DB, "bytes", 'b', 1000);
  db = dbcreate (DB, "pointers", 'p', 1000);
  db = dbcreate (DB, "mol_atom_xyz", 'x', 10);
  DBlist (DB);

}

