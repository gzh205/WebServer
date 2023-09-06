#pragma once
#define RegType(name) Reflection::TypeInfo::type_map[#name] = new TypeInfo();
#define RegF(name) type.fields[#name] = Reflection::FieldInfo(#name,shared_ptr<string>(&name));
#define RegC(cc) type.name=#cc;type.addr=this;