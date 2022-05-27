#include <QCoreApplication>
#include <cmath>
#include <optional>

enum class LinesStatus {
  INSIDE,
  INTERSECT,
  PARTIALLY_INTERSECT,
  OUTSIDE
};

struct Point {
  float x = NAN,y = NAN;

  bool isValid() const{
    return(x != NAN && y != NAN);
  }
};


class Line {
public:
  Line(){}
  Line(const Point& point1) : m_point1(point1) {}
  Line(const Point& point1 ,const Point& point2) : m_point1(point1) , m_point2(point2){}

  const Point& startPoint() const{
  return m_point1;
  }

  const Point& endPoint() const{
  return m_point2;
  }

  void setStartPoint(const Point& point){
   m_point1 = point;
  }

  void setEndPoint(const Point& point){
   m_point2 = point;
  }

  bool intersect(const Line& anotherLine) const{

    //    Отрезки пересекаются когда:
    //   1)Пересекаются прямоугольники, которые их ограничивают;
    //   2)[AC x AB]·[AD x AB] ≤ 0;
    //   3)[CA x CD]·[CB x CD] ≤ 0.


    if(!rectangleIntersect(anotherLine) ) {
        return false;
      }


    //A - начальная точка
    //B - конечная точка

    //C - начальная точка другого отрезка
    //D - конечная точка другого отрезка

    //координаты вектора
    float ABx, ABy, ACx, ACy, ADx, ADy;
    float CAx, CAy, CBx, CBy, CDx, CDy;


    float ACxAB, ADxAB, CAxCD, CBxCD;

    ACx = anotherLine.m_point1.x - m_point1.x; ACy = anotherLine.m_point1.y - m_point1.y;

    ABx = m_point2.x - m_point1.x; ABy = m_point2.y - m_point1.y;

    ADx = anotherLine.m_point2.x - m_point1.x; ADy = anotherLine.m_point2.y - m_point1.y;

    CAx = m_point1.x - anotherLine.m_point1.x; CAy = m_point1.y - anotherLine.m_point1.y;

    CBx = m_point2.x - anotherLine.m_point1.x; CBy = m_point2.y - anotherLine.m_point1.y;

    CDx = anotherLine.m_point2.x - anotherLine.m_point1.x; CDy = anotherLine.m_point2.y - anotherLine.m_point1.y ;

    ACxAB = ACx * ABy - ACy * ABx;

    ADxAB = ADx * ABy - ADy * ABx;

    CAxCD = CAx * CDy - CAy * CDx;

    CBxCD = CBx * CDy - CBy * CDx;

    return ACxAB * ADxAB <= 0 && CAxCD * CBxCD <= 0;

    return 1;

  }


  bool isValid()const{

   return (m_point1.isValid() , m_point2.isValid());
  }

private:

  bool rectangleIntersect(const Line& anotherLine) const{

    //создаем ограничивающий прямоугольник для отрезка ( bounding rect)
    float x1 = std::min(m_point1.x ,m_point2.x );
    float y1 = std::min(m_point1.y ,m_point2.y);
    float x2 = std::max(m_point1.x ,m_point2.x );
    float y2 = std::max(m_point1.y ,m_point2.y);

    float x3 = std::min(anotherLine.m_point1.x ,anotherLine.m_point2.x );
    float y3 = std::min(anotherLine.m_point1.y ,anotherLine.m_point2.y );
    float x4 = std::max(anotherLine.m_point1.x ,anotherLine.m_point2.x );
    float y4 = std::max(anotherLine.m_point1.y ,anotherLine.m_point2.y );


    //проверка на пересечение
    if ((x3 - x2)*(x4 - x1) > 0 ||

        ((y3 - y2)*(y4 - y1) > 0)){
        return 0;
      }

    return 1;

  }

private:
  Point m_point1;
  Point m_point2;
};



class Polygon {

  void addPoint(const Point& point){
  if(point.isValid())
    m_vertices.push_back(point);
  }


  std::optional<LinesStatus> lineStatus(const Line& line) const{


    if(!isValid()){
      return std::nullopt;
      }

   Line linePolygon;
   for(const Point& point : m_vertices){

       // для самой первой итерации
       if(!linePolygon.isValid()){
         linePolygon.setStartPoint(point);
         continue;
         }

       linePolygon.setEndPoint(point);

     }



  }

  bool isValid() const{

  if(m_vertices.size() < 3)
   return false;

  return true;

  }

private:
// std::optional<std::reference_wrapper<const Point>> lastPoint() const{

//   if(m_lines.empty()){
//     return std::nullopt;
//     }

//  return m_lines.back().endPoint();
// }


//  std::vector<Line> m_lines;

  std::vector<Point> m_vertices;

};













int main(int argc, char *argv[])
{
  QCoreApplication a(argc, argv);

  return a.exec();
}
