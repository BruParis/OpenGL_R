#include "Scene.h"
#include "opengl.h"
#include <GL/gl.h>
#include <iostream>
#include <vector>

void draw_axes() {
  glLineWidth(5);
  glBegin(GL_LINES);

  glColor3f(1.f, 0.f, 0.f);
  glVertex3f(0.f, 0.f, 0.f);
  glVertex3f(1.f, 0.f, 0.f);

  glColor3f(0.f, 1.f, 0.f);
  glVertex3f(0.f, 0.f, 0.f);
  glVertex3f(0.f, 1.f, 0.f);

  glColor3f(0.f, 0.f, 1.f);
  glVertex3f(0.f, 0.f, 0.f);
  glVertex3f(0.f, 0.f, 1.f);

  glEnd();
}

void draw_grid() {
  int i;
  glLineWidth(1);
  glColor3f(1.f, 1.f, 1.f);

  glPushMatrix();
  glTranslatef(-5, 0, -5);

  glBegin(GL_LINES);

  for (i = 0; i <= 10; ++i) {
    glVertex3f(i, 0, 0);
    glVertex3f(i, 0, 10);
  }

  for (i = 0; i <= 10; ++i) {
    glVertex3f(0, 0, i);
    glVertex3f(10, 0, i);
  }

  glEnd();
  glPopMatrix();
}

void draw_cube() {
  static float pt[8][3] = {{0, 0, 0}, {1, 0, 0}, {1, 0, 1}, {0, 0, 1},
                           {0, 1, 0}, {1, 1, 0}, {1, 1, 1}, {0, 1, 1}};
  static int f[6][4] = {{0, 1, 2, 3}, {5, 4, 7, 6}, {1, 5, 6, 2},
                        {0, 3, 7, 4}, {3, 2, 6, 7}, {0, 4, 5, 1}};
  static float n[6][3] = {{0, -1, 0}, {0, 1, 0}, {1, 0, 0},
                          {-1, 0, 0}, {0, 0, 1}, {0, 0, -1}};
  static float uv[6][4][2] = {
      {{0, 0}, {1, 0}, {1, 1}, {0, 1}}, {{0, 0}, {1, 0}, {1, 1}, {0, 1}},
      {{0, 0}, {1, 0}, {1, 1}, {0, 1}}, {{0, 0}, {1, 0}, {1, 1}, {0, 1}},
      {{0, 0}, {1, 0}, {1, 1}, {0, 1}}, {{0, 0}, {1, 0}, {1, 1}, {0, 1}}};
  glTranslatef(-0.5, -0.5, -0.5);
  glBegin(GL_QUADS);
  for (int i = 0; i < 6; i++) {
    glNormal3f(n[i][0], n[i][1], n[i][2]);
    for (int j = 0; j < 4; j++) {
      glTexCoord2f(uv[i][j][0], uv[i][j][1]);
      glVertex3f(pt[f[i][j]][0], pt[f[i][j]][1], pt[f[i][j]][2]);
    }
  }
  glEnd();
}

void computeHalfVertex(const Vec3f *v1, const Vec3f *v2, Vec3f &newV,
                       float radius) {
  newV.x = (v1->x + v2->x);
  newV.y = (v1->y + v2->y);
  newV.z = (v1->z + v2->z);
  float scale = radius / vecLength(newV);
  Vec3f aux;
  vecMul(aux, scale, newV);
  newV = aux;
}

void addIndices(int index0, int index1, int index2, std::vector<int> &indices) {
  indices.push_back(index0);
  indices.push_back(index1);
  indices.push_back(index2);
}

void subdivideTriangle(std::vector<Vec3f> &ptVec, std::vector<int> &indices,
                       int subdivision, float r) {
  indices = {0, 1, 2};
  std::vector<int> tmpIndices;
  const Vec3f *v1, *v2, *v3; // ptr to original vertices of a triangle
  Vec3f newV1, newV2, newV3;
  unsigned int index;

  // iterate all subdivision levels
  for (int i = 1; i <= subdivision; ++i) {
    // copy index arrays and clear
    tmpIndices = indices;
    indices.clear();

    // perform subdivision for each triangle
    for (int j = 0; j < tmpIndices.size(); j += 3) {
      // get 3 vertices of a triangle
      v1 = &ptVec[tmpIndices[j]];
      v2 = &ptVec[tmpIndices[j + 1]];
      v3 = &ptVec[tmpIndices[j + 2]];

      // compute 3 new vertices by spliting half on each edge
      //         v1
      //        / \       
      // newV1 *---* newV3
      //      / \ / \     
      //    v2---*---v3
      //       newV2
      computeHalfVertex(v1, v2, newV1, r);
      computeHalfVertex(v2, v3, newV2, r);
      computeHalfVertex(v1, v3, newV3, r);

      // add 3 new points to point Vec
      int numPt = ptVec.size();
      ptVec.push_back(newV1);
      ptVec.push_back(newV2);
      ptVec.push_back(newV3);

      // add new indices
      addIndices(tmpIndices[j], numPt, numPt + 2, indices);
      addIndices(tmpIndices[j + 1], numPt + 1, numPt, indices);
      addIndices(tmpIndices[j + 2], numPt + 2, numPt + 1, indices);
      addIndices(numPt, numPt + 1, numPt + 2, indices);
    }
  }
}

void draw_sphere(float r, float nlat, float nlong) {
  // First, define the icosahedron inscribed in the sphere.
  const float rect_h = 2 * r / sqrt(1 + 1 / GOLDEN_RATIO);
  const float rect_w = rect_h / GOLDEN_RATIO;

  Vec3f x_axis, y_axis, z_axis;
  vecInit(x_axis, 1.0f, 0.0f, 0.0f);
  vecInit(y_axis, 0.0f, 1.0f, 0.0f);
  vecInit(z_axis, 0.0f, 0.0f, 1.0f);

  std::vector<Vec3f> fst_rect;
  Vec3f vur, vul, vdl, vdr;
  vecInit(vur, rect_h / 2, rect_w / 2, 0.0f);
  vecInit(vul, -rect_h / 2, rect_w / 2, 0.0f);
  vecInit(vdl, -rect_h / 2, -rect_w / 2, 0.0f);
  vecInit(vdr, rect_h / 2, -rect_w / 2, 0.0f);

  fst_rect.push_back(vur);
  fst_rect.push_back(vul);
  fst_rect.push_back(vdl);
  fst_rect.push_back(vdr);

  float angle = M_PI / 2.0f;

  // glBegin(GL_QUADS);
  // for (int i = 0; i < 4; i++)
  //   glVertex3f(fst_rect[i].x, fst_rect[i].y, fst_rect[i].z);
  // glEnd();

  std::vector<Vec3f> snd_rect;
  for (int i = 0; i < 4; i++) {
    Vec3f rotated;
    float angle = M_PI / 2.0f;
    vecRotation(rotated, fst_rect[i], z_axis, angle);
    vecRotation(rotated, rotated, x_axis, angle);
    snd_rect.push_back(rotated);
  }

  // glBegin(GL_QUADS);
  // for (int i = 0; i < 4; i++)
  //   glVertex3f(snd_rect[i].x, snd_rect[i].y, snd_rect[i].z);
  // glEnd();

  std::vector<Vec3f> trd_rect;
  for (int i = 0; i < 4; i++) {
    Vec3f rotated;
    float angle = M_PI / 2.0f;
    vecRotation(rotated, fst_rect[i], y_axis, angle);
    vecRotation(rotated, rotated, x_axis, angle);
    trd_rect.push_back(rotated);
  }

  // glBegin(GL_QUADS);
  // for (int i = 0; i < 4; i++)
  //   glVertex3f(trd_rect[i].x, trd_rect[i].y, trd_rect[i].z);
  // glEnd();

  std::vector<std::vector<Vec3f>> rects;
  rects.push_back(fst_rect);
  rects.push_back(snd_rect);
  rects.push_back(trd_rect);

  static int order[20][3][2] = {
      {{0, 0}, {2, 0}, {2, 3}}, {{0, 0}, {2, 3}, {1, 0}},
      {{0, 0}, {1, 0}, {0, 3}}, {{0, 0}, {0, 3}, {1, 1}},
      {{0, 0}, {1, 1}, {2, 0}}, {{1, 3}, {0, 1}, {0, 2}},
      {{1, 3}, {0, 2}, {2, 2}}, {{1, 3}, {2, 2}, {1, 0}},
      {{1, 3}, {1, 0}, {2, 3}}, {{1, 3}, {2, 3}, {0, 1}},
      {{1, 2}, {0, 1}, {0, 2}}, {{1, 2}, {0, 2}, {2, 1}},
      {{1, 2}, {2, 1}, {1, 1}}, {{1, 2}, {1, 1}, {2, 0}},
      {{1, 2}, {2, 0}, {0, 1}}, {{2, 3}, {2, 0}, {0, 1}},
      {{2, 1}, {2, 2}, {0, 3}}, {{2, 1}, {0, 3}, {1, 1}},
      {{2, 1}, {2, 2}, {0, 2}}, {{2, 2}, {0, 3}, {1, 0}}};

  for (int i = 0; i < 20; i++) {
    int rect_0 = order[i][0][0];
    int pt_0 = order[i][0][1];
    int rect_1 = order[i][1][0];
    int pt_1 = order[i][1][1];
    int rect_2 = order[i][2][0];
    int pt_2 = order[i][2][1];

    Vec3f fst_pt = rects[rect_0][pt_0];
    Vec3f snd_pt = rects[rect_1][pt_1];
    Vec3f trd_pt = rects[rect_2][pt_2];

    std::vector<int> indices;
    std::vector<Vec3f> ptVec;
    ptVec.push_back(fst_pt);
    ptVec.push_back(snd_pt);
    ptVec.push_back(trd_pt);

    subdivideTriangle(ptVec, indices, 5, r);
    glBegin(GL_TRIANGLES);
    for (int j = 0; j < indices.size(); j += 3) {
      Vec3f ptArray[3] = {ptVec[indices[j]], ptVec[indices[j + 1]],
                          ptVec[indices[j + 2]]};

      glNormal3f(ptArray[0].x, ptArray[0].y, ptArray[0].z);
      glVertex3f(ptArray[0].x, ptArray[0].y, ptArray[0].z);
      glNormal3f(ptArray[1].x, ptArray[1].y, ptArray[1].z);
      glVertex3f(ptArray[1].x, ptArray[1].y, ptArray[1].z);
      glNormal3f(ptArray[2].x, ptArray[2].y, ptArray[2].z);
      glVertex3f(ptArray[2].x, ptArray[2].y, ptArray[2].z);

    }
    glEnd();
  }
}

void draw_cylinder() {}

void draw_cone() {}

void sceneDraw() {
  // A MODIFIER : CETTE FONCTION AFFICHE LA SCENE PRINCIPALE

  // Mon 1er cube
  glColor3f(1, 0.6, 0.1);
  glPushMatrix(); // sauvegarde la transformation courante
  glEnable(GL_TEXTURE_2D);
  glTranslatef(0, 2, 0);
  glColor3f(1, 0.6, 0.1);
  draw_sphere(1, 36, 36);

  glTranslatef(6, 0, 2);
  glColor3f(0, 0.5, 0.8);
  draw_sphere(0.5, 15, 15);

  glTranslatef(0, 0, 3);
  glColor3f(0, 0.2, 0.8);
  draw_sphere(0.7, 15, 15);

  glDisable(GL_TEXTURE_2D);
  glPopMatrix();

  // **************** FORME DE BASE ***********************/
}
