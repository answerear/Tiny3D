/*******************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
 * Copyright (C) 2015-2020  Answer Wong
 * For latest info, see https://github.com/answerear/Tiny3D
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ******************************************************************************/



/*
 * Model 文件格式如下：
 *  {
 *      "header":
 *      {
 *          "magic": "T3D",
 *          "type": "MDL",
 *          "version": 0x00000100,
 *      },
 *      "model":
 *      {
 *          "id": "body",
 *          "mesh":
 *          {
 *              "vertices":
 *              [
 *                  "attributes":
 *                  {
 *                      "id": "0",
 *                      [
 *                          {
 *                              "id": "POSITION",
 *                              "size": 3,
 *                              "type": "float"
 *                          },
 *                          {
 *                              "id": "NORMAL",
 *                              "size": 3,
 *                              "type": "float"
 *                          },
 *                          {
 *                              "id": "BLEND_WEIGHT",
 *                              "size": 4,
 *                              "type": "float"
 *                          },
 *                          {
 *                              "id": "BLEND_INDEX",
 *                              "size": 4,
 *                              "type": "ushort"
 *                          }
 *                      ]
 *                  },
 *                  "data":
 *                  [
 *                      -1.0, 1.0, 1.0,     // POSIITON
 *                      1.0, 1.0, 1.0,      // NORMAL  
 *                      0.5, 0.3, 0.2, 0.0, // BLEND_WEIGHT
 *                      1, 2, 3, 4          // BLEND_INDEX
 *                  ]
 *              ],
 *              "submeshes":
 *              [
 *                  {
 *                      "id": "body",
 *                      "
 *                  }
 *              ]
 *          }
 *      }
 *  }
 */

