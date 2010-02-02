#ifndef PARSERS_H
#define PARSERS_H

#define DATA(node) xmlNodeListGetString(node->doc, node->children, 1)
#define NODE_IS_TYPE(node, type) xmlStrEqual (node->name, (const xmlChar *) type)


#define XML_WEATHER_DAYF_N 5
#define XML_WEATHER_LINK_N 4

struct xml_weather
{
        struct xml_loc *loc;
        struct xml_lnks *lnks;
        struct xml_cc *cc;		
        struct xml_dayf *dayf[XML_WEATHER_DAYF_N];
};

struct xml_loc
{
        int8 *dnam;
        int8 *sunr;
        int8 *suns;
};

struct xml_lnks
{
        int8 *l[XML_WEATHER_LINK_N];
        int8 *t[XML_WEATHER_LINK_N];
};

struct xml_uv 
{
        int8 *i;
        int8 *t;
};

struct xml_wind
{
        int8 *s;
        int8 *gust;
        int8 *d;
        int8 *t;
};

struct xml_bar
{
        int8 *r;
        int8 *d;
};

struct xml_cc
{
        int8 *lsup;
        int8 *obst;
        int8 *flik;
        int8 *t;
        int8 *icon;
        int8 *tmp;
        
        int8 *hmid;
        int8 *vis;
        
        struct xml_uv *uv;
        struct xml_wind *wind;
        struct xml_bar *bar;

        int8 *dewp;
};

struct xml_part
{
        int8 *icon;
        int8 *t;
        int8 *ppcp;
        int8 *hmid;

        struct xml_wind *wind;
};

struct xml_dayf
{
        int8 *day;
        int8 *date;

        int8 *hi;
        int8 *low;

        struct xml_part *part[2];
};

struct xml_weather *parse_weather(xmlNode *cur_node);
struct xml_loc *parse_loc(xmlNode *cur_node);
struct xml_lnks *parse_lnks(xmlNode *cur_node);
struct xml_cc *parse_cc(xmlNode *cur_node);
struct xml_dayf *parse_dayf(xmlNode *cur_node);

void xml_weather_free(struct xml_weather *);
#endif
