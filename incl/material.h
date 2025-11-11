#pragma once
#include "objects.h"
#include "hit.h"

class hit_record; 

class material {
  public:
    virtual ~material()=default;

    virtual bool scratter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const{
      return false;
    }
};
