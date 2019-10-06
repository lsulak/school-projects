{ Zajisteni zakladni kompatibility IFJ14-FreePascal }
{$H+}                    {Aktivuje neomezene retezce}
type integer = longint;      {Podpora 32-bit Integer}
type real = double;          {Podpora 64-bit Real}
             
function sort(str : string) : string;
{Bubble Sort}
var i, j, n : integer;
    finished : boolean;
    swap : char;
begin
  sort := str;
  n := Length(sort);
  i := 2;
  repeat
    finished := true;
    for j := n downto i do
      if sort[j-1] > sort[j] then
      begin
        swap := sort[j-1];
        sort[j-1] := sort[j];
        sort[j] := swap;
        finished := false
      end;
    i := i + 1
  until finished or (i = (n + 1))
end;

function find(haystack : string; needle : string) : integer;
begin
  find := Pos(needle, haystack)
end;

{ Zde bude nasledovat program jazyka IFJ14 }